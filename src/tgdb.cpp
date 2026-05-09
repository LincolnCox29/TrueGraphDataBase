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
    if (size() >= capacity_)
        expand();
    node_id id = next_id_++;
    new (&base_[id]) Node();
    base_[id].set_type(t);
    base_[0].set_raw_value(size());
    return id;
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
        base_[0].set_raw_value(next_id_);
    }
    else 
    {
        mmap_ = std::make_unique<mio::mmap_sink>(
            mio::make_mmap_sink(path, 0, mio::map_entire_file, ec)
        );
        if (ec) throw std::runtime_error(ec.message());

        base_ = reinterpret_cast<Node*>(mmap_->data());
        capacity_ = mmap_->size() / sizeof(Node);
        next_id_ = base_[0].raw_value();
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