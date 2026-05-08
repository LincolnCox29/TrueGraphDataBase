#pragma once
#include "tgdbTypes.h"
#include <iostream>

using node_id = uint64_t;
constexpr node_id NULL_NODE = 0;

class Node {
public:
    Node();

    node_id parent() const;
    void set_parent(node_id id);

    node_id child() const;
    void set_child(node_id id);

    node_id prev() const;
    void set_prev(node_id id);

    node_id next() const;
    void set_next(node_id id);

    node_id name() const;
    void set_name(node_id id);

    uint64_t raw_value() const;
    void set_raw_value(uint64_t v);

    Type type() const;
    void set_type(Type t);

private:
    node_id parent_;
    node_id child_;
    node_id prev_;
    node_id next_;
    node_id name_;
    uint64_t value_;
    Type type_;
};