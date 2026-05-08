#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include "tgdb_types.h"
#include "node.h"

static_assert(sizeof(Node) == 56, "Node size must be 56 bytes");

class TGDB 
{
private:
    std::vector<Node> nodes;

    node_id create_string_internal(const std::string& s);

    std::string read_string(node_id id) const;

    node_id alloc(Type t);

public:
    TGDB();

    void print_node(node_id id);

    Node& get(node_id id) {
        if (id >= nodes.size())
            throw std::out_of_range("node_id out of range");
        return nodes[id];
    }
    const Node& get(node_id id) const {
        if (id >= nodes.size())
            throw std::out_of_range("node_id out of range");
        return nodes[id];
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
};

template<> node_id TGDB::create<int>(const int&);
template<> node_id TGDB::create<double>(const double&);
template<> node_id TGDB::create<std::string>(const std::string&);
template<> int TGDB::get<int>(node_id) const;
template<> double TGDB::get<double>(node_id) const;
template<> std::string TGDB::get<std::string>(node_id) const;