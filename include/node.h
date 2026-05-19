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
#include "tgdb_types.h"
#include <iostream>

using node_id = uint64_t;
constexpr node_id NULL_NODE = 0;

class Node 
{
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

    uint64_t padding__;
};