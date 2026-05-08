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

#include "node.h"
#include "tgdb.h"

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