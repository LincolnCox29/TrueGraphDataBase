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

#pragma once
//#define DEBUG
#define NOMINMAX
#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include "tgdb_types.h"
#include "node.h"
#include "mio/mio.hpp"

static_assert(sizeof(Node) == 56, "Node size must be 56 bytes");

class TGDB 
{
private:

    std::unique_ptr<mio::mmap_sink> mmap_;
    Node* base_ = nullptr;
    uint64_t capacity_ = 0;
    uint64_t next_id_ = 1;
    node_id dealloc_sequence_id;

    std::string filepath_;

    std::string read_string(node_id id) const;

    node_id alloc(Type t);

    void dealloc(node_id);

    node_id pop_dealloc_seq();

    void expand();

    void pack_char_chunk(node_id str_id, const std::string& str);
    std::string unpack_char_chunk(node_id str_id) const;

public:

    size_t __live_nodes_debug();

    ~TGDB();
    TGDB(const std::string& path, uint64_t initial_capacity);

    inline bool is_dealloced(node_id id) const { return get(id).type() == Type::DELETED; }

    void close() { delete this; };

    void sync();

    void print_node(node_id id);

    void delete_node(node_id);

    Node& get(node_id id) 
    {
        if (id >= size()) 
            throw std::out_of_range("node_id is out of range");
        return base_[id];
    }

    const Node& get(node_id id) const
    {
        if (id >= size()) 
            throw std::out_of_range("node_id is out of range");
        return base_[id];
    }

    std::string node_name(node_id id);

    void set_node_name(node_id id, const std::string& name);

    template<typename T>
    node_id create(const T& value);

    template<typename T>
    T get(node_id id) const;

    node_id create_object(const std::string& type_name);

    void add_property(node_id obj, const std::string& key, node_id value_id);

    node_id get_property(node_id obj, const std::string& key);

    inline size_t size() const { return next_id_; };

    node_id first_by_name(const std::string& name);
    node_id first_by_name(node_id start, const std::string& name);

    std::vector<node_id> all_by_name(const std::string& name);
    std::vector<node_id> all_by_name(node_id start, const std::string& name);

};

template<> node_id TGDB::create<int>(const int&);
template<> node_id TGDB::create<double>(const double&);
template<> node_id TGDB::create<std::string>(const std::string&);
template<> int TGDB::get<int>(node_id) const;
template<> double TGDB::get<double>(node_id) const;
template<> std::string TGDB::get<std::string>(node_id) const;