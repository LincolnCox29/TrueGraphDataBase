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

std::string TGDB::node_name(node_id id)
{
    node_id name_id = get(id).name();
    if (name_id == NULL_NODE) 
        return "";
    return read_string(name_id);
}

void TGDB::set_node_name(node_id id, const std::string& name)
{
    get(id).set_name(create<std::string>(name));
}

node_id TGDB::create_string_internal(const std::string& s) 
{
    node_id str_id = alloc(Type::STRING);
    if (s.empty()) return str_id;

    std::vector<node_id> chars;
    for (char c : s) 
    {
        node_id char_id = alloc(Type::CHAR);
        get(char_id).set_raw_value(static_cast<uint8_t>(c));
        chars.push_back(char_id);
    }
    for (size_t i = 0; i < chars.size() - 1; ++i)
        get(chars[i]).set_next(chars[i + 1]);

    get(str_id).set_child(chars[0]);
    return str_id;
}

std::string TGDB::read_string(node_id id) const 
{
    const Node& n = get(id);
    if (n.type() != Type::STRING)
        throw std::runtime_error("Not a string");
    std::string result;
    node_id cur = n.child();
    while (cur != 0) 
    {
        const Node& c = get(cur);
        result.push_back(static_cast<char>(c.raw_value()));
        cur = c.next();
    }
    return result;
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
    Node& node = get(id);

    node.set_child(NULL_NODE);
    node.set_parent(NULL_NODE);
    node.set_next(NULL_NODE);
    node.set_prev(NULL_NODE);
    node.set_name(NULL_NODE);
    node.set_type(Type::SYSTEM);

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
    node_id first_child = node.child();

    if (prev_id != NULL_NODE) 
    {
        Node& prev = get(prev_id);
        prev.set_next(next_id);
    }
    if (next_id != NULL_NODE) 
    {
        Node& next = get(next_id);
        next.set_prev(prev_id);
    }
    if (parent_id != NULL_NODE) 
    {
        Node& parent = get(parent_id);
        if (parent.child() == id)
            parent.set_child(next_id);
    }

    std::vector<node_id> descendants;
    std::function<void(node_id)> collect = [&](node_id start) 
    {
        node_id cur = start;
        while (cur != NULL_NODE) 
        {
            descendants.push_back(cur);
            Node& c = get(cur);

            node_id name_id = c.name();
            collect(c.name());
            if (c.child() != NULL_NODE)
                collect(c.child());
            cur = c.next();
        }
    };
    if (node.name() != NULL_NODE)
        collect(node.name());
    if (first_child != NULL_NODE) 
        collect(first_child);


    node.set_child(NULL_NODE);

    for (node_id nid : descendants) 
    {
        dealloc(nid);
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
    return create_string_internal(value);
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