#include "node.h"
#include "db.h"

Node::Node() : parent_(0), child_(0), prev_(0), next_(0), name_(0), value_(0), type_(Type::INT) {}

node_id Node::parent() const { return parent_; };
void Node::set_parent(node_id id) { parent_ = id; }

node_id Node::child() const { return child_; }
void Node::set_child(node_id id) { child_ = id; }

node_id Node::prev() const { return prev_; }
void Node::set_prev(node_id id) { prev_ = id; }

node_id Node::next() const { return next_; }
void Node::set_next(node_id id) { next_ = id; }

node_id Node::name() const { return name_; };
void Node::set_name(node_id id) { name_ = id; };

uint64_t Node::raw_value() const { return value_; }
void Node::set_raw_value(uint64_t v) { value_ = v; }

Type Node::type() const { return type_; }
void Node::set_type(Type t) { type_ = t; }