/*
* MIT License
*
* Copyright (c) 2026 LincolnCox29
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "tgdb.h"
#include <string>
#include <fstream>
#include <functional>
#include <unordered_set>
#define TGDB_CHUNK_SIZE 8

std::string TGDB::node_name(node_id id)
{
    node_id name_id = get(id).name();
    if (name_id == NULL_NODE) 
        return "";
    return read_string(name_id);
}

void TGDB::set_node_name(node_id id, const std::string& name)
{
    Node& node = get(id);
    dealloc(node.name());
    node.set_name(create<std::string>(name));
}

std::string TGDB::read_string(node_id id) const
{
    const Node& n = get(id);
    if (n.type() != Type::STRING)
        throw std::runtime_error("Not a string");
    return unpack_char_chunk(id);
}

size_t TGDB::__live_nodes_debug()
{
    std::unordered_set<node_id> freed;
    node_id cur = get(dealloc_sequence_id).child();
    while (cur != NULL_NODE)
    {
        freed.insert(cur);
        cur = get(cur).next();
    }

    size_t count = 0;
    for (node_id id = 1; id < next_id_; ++id)
    {
        if (freed.find(id) == freed.end()) count++;
    }
    return count;
}

node_id TGDB::alloc(Type t)
{
    node_id mem = pop_dealloc_seq();
    if (mem != NULL_NODE)
    {
        new (&base_[mem]) Node();
        base_[mem].set_type(t);
        return mem;
    }

    if (size() >= capacity_)
        expand();
    node_id id = next_id_++;
    new (&base_[id]) Node();
    base_[id].set_type(t);
    base_[0].set_raw_value(size());
    return id;
}

node_id TGDB::pop_dealloc_seq()
{
    Node& dealloc_seq = get(dealloc_sequence_id);
    node_id top_id = dealloc_seq.child();
    if (top_id == NULL_NODE) return NULL_NODE;
    Node& top = get(top_id);
    node_id next_id = top.next();

#ifdef DEBUG
    std::cout << "Reuse this shit! :\n";
    print_node(dealloc_sequence_id);
#endif // DEBUG

    dealloc_seq.set_child(next_id);
    top.set_next(NULL_NODE);
    return top_id;
}

void TGDB::dealloc(node_id id) 
{
    if (id == NULL_NODE) return;
    Node& node = get(id);

    if (node.name() != NULL_NODE) 
    {
        dealloc(node.name());
    }

    if (node.child() != NULL_NODE) 
    {
        node_id cur = node.child();
        while (cur != NULL_NODE) 
        {
            node_id next = get(cur).next();
            dealloc(cur);
            cur = next;
        }
    }

    node.set_child(NULL_NODE);
    node.set_parent(NULL_NODE);
    node.set_next(NULL_NODE);
    node.set_prev(NULL_NODE);
    node.set_name(NULL_NODE);
    node.set_type(Type::DELETED);
    node.set_raw_value(0);

    Node& dealloc_seq = get(dealloc_sequence_id);
    node_id old_head = dealloc_seq.child();
    node.set_next(old_head);
    dealloc_seq.set_child(id);
}

void TGDB::delete_node(node_id id)
{
    if (id == NULL_NODE) return;
    Node& node = get(id);

    node_id parent_id = node.parent();
    node_id prev_id = node.prev();
    node_id next_id = node.next();

    if (prev_id != NULL_NODE) get(prev_id).set_next(next_id);
    if (next_id != NULL_NODE) get(next_id).set_prev(prev_id);
    if (parent_id != NULL_NODE) 
    {
        Node& parent = get(parent_id);
        if (parent.child() == id) parent.set_child(next_id);
    }

    dealloc(id);
}

TGDB::TGDB(const std::string& path, uint64_t initial_capacity)
    : filepath_(path)
{
    std::ifstream check(path, std::ios::binary);
    bool exists = check.good();
    check.close();

    std::error_code ec;

    if (!exists) 
    {
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        file.seekp(initial_capacity * sizeof(Node) - 1);
        file.write("", 1);
        file.close();

        mmap_ = std::make_unique<mio::mmap_sink>(
            mio::make_mmap_sink(path, 0, initial_capacity * sizeof(Node), ec)
        );
        if (ec) throw std::runtime_error(ec.message());

        base_ = reinterpret_cast<Node*>(mmap_->data());
        capacity_ = initial_capacity;
        next_id_ = 1;

        new (&base_[0]) Node();
        base_[NULL_NODE].set_raw_value(next_id_);

        node_id dealloc_seq_id = create_object("SYS_dealloc_seq");
        base_[NULL_NODE].set_child(dealloc_seq_id);
        this->dealloc_sequence_id = dealloc_seq_id;
    }
    else 
    {
        mmap_ = std::make_unique<mio::mmap_sink>(
            mio::make_mmap_sink(path, 0, mio::map_entire_file, ec)
        );
        if (ec) throw std::runtime_error(ec.message());

        base_ = reinterpret_cast<Node*>(mmap_->data());
        capacity_ = mmap_->size() / sizeof(Node);
        next_id_ = base_[NULL_NODE].raw_value();
        dealloc_sequence_id = base_[NULL_NODE].child();
    }
}

TGDB::~TGDB() 
{
    try 
    {
        std::error_code ec;
        mmap_->sync(ec);
    }
    catch (...) { }
}

template<>
node_id TGDB::create<int>(const int& value) 
{
    node_id id = alloc(Type::INT);
    get(id).set_raw_value(static_cast<uint64_t>(value));
    return id;
}

template<>
node_id TGDB::create<double>(const double& value) 
{
    node_id id = alloc(Type::FLOAT);
    uint64_t raw;
    std::memcpy(&raw, &value, sizeof(raw));
    get(id).set_raw_value(raw);
    return id;
}

template<>
node_id TGDB::create<std::string>(const std::string& value) 
{
    node_id str_id = alloc(Type::STRING);
    if (value.empty())
        return str_id;

    pack_char_chunk(str_id, value);

    return str_id;
}

template<>
int TGDB::get<int>(node_id id) const 
{
    const Node& n = get(id);
    if (n.type() != Type::INT)
        throw std::runtime_error("Not an integer");
    return static_cast<int>(n.raw_value());
}

template<>
double TGDB::get<double>(node_id id) const 
{
    const Node& n = get(id);
    if (n.type() != Type::FLOAT)
        throw std::runtime_error("Not a float");
    double d;
    uint64_t raw = n.raw_value();
    std::memcpy(&d, &raw, sizeof(d));
    return d;
}

template<>
std::string TGDB::get<std::string>(node_id id) const 
{
    return read_string(id);
}

node_id TGDB::create_object(const std::string& type_name) 
{
    node_id obj = alloc(Type::STRUCT);
    set_node_name(obj, type_name);
    return obj;
}

void TGDB::add_property(node_id obj, const std::string& key, node_id prop_id)
{
    set_node_name(prop_id, key);
    Node& p = get(prop_id);
    p.set_parent(obj);
    Node& obj_node = get(obj);
    p.set_next(obj_node.child());

    node_id next_id = p.next();
    if (next_id != NULL_NODE) 
    {
        Node& next = get(next_id);
        next.set_prev(prop_id);
    }

    obj_node.set_child(prop_id);
}

node_id TGDB::get_property(node_id obj, const std::string& key)
{
    node_id cur = get(obj).child();
    while (cur != 0) {
        const Node& prop = get(cur);
        if (prop.type() == Type::STRUCT && node_name(cur) == key)
            return prop.child();
        cur = prop.next();
    }
    return 0;
}

void TGDB::pack_char_chunk(node_id str_id, const std::string& str)
{
    size_t size = str.size();
    get(str_id).set_raw_value(size);

    size_t chunk_count = (size + TGDB_CHUNK_SIZE - 1) / TGDB_CHUNK_SIZE;
    if (chunk_count == 0) return;

    node_id prev = NULL_NODE;
    node_id first = NULL_NODE;

    for (size_t i = 0; i < chunk_count; ++i)
    {
        node_id chunk = alloc(Type::INT);

        if (prev != NULL_NODE)
            get(prev).set_next(chunk);
        else
            first = chunk;

        uint64_t blob = 0;
        size_t offset = i * TGDB_CHUNK_SIZE;
        size_t len = std::min(static_cast<size_t>(TGDB_CHUNK_SIZE), size - offset);
        memcpy(&blob, str.data() + offset, len);
        get(chunk).set_raw_value(blob);

        prev = chunk;
    }

    get(str_id).set_child(first);
}

std::string TGDB::unpack_char_chunk(node_id str_id) const
{
    const Node& str_node = get(str_id);
    size_t size = str_node.raw_value();
    std::string out;

    node_id cur = str_node.child();
    while (cur != NULL_NODE)
    {
        uint64_t blob = get(cur).raw_value();
        for (int c = 0; c < TGDB_CHUNK_SIZE; ++c)
            out.push_back(static_cast<char>((blob >> (c * 8)) & 0xFF));
        cur = get(cur).next();
    }

    out.resize(size);
    return out;
}