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
#include <functional>
#include <iostream>
#include <fstream>

void TGDB::print_node(node_id id)
{
    std::string output;
    int tabs = 0;

    std::function<void(node_id, std::string&, int&)> formStr;
    formStr = [&](node_id cur_id, std::string& out, int& t)
    {
        node_id cur = cur_id;

        while (cur != NULL_NODE)
        {
            Type type = get(cur).type();
            if (type != Type::CHAR)
            {
                for (int i = 0; i < t; ++i) out += "\t";
                out += node_name(cur) + " : ";
                switch (type)
                {
                    case Type::STRUCT: out += "{"; break;
                    case Type::INT: out += std::to_string(get<int>(cur)); break;
                    case Type::FLOAT: out += std::to_string(get<double>(cur)); break;
                    case Type::STRING: out += read_string(cur); break;
                }
                out += "\n";
            }

            node_id child = get(cur).child();
            if (child != NULL_NODE)
            {
                ++t;
                formStr(child, out, t);
                --t;
            }

            if (get(cur).type() == Type::STRUCT)
            {
                for (int i = 0; i < t; ++i) out += "\t";
                out += "}\n";
            }

            cur = get(cur).next();
        }
    };

    Node& node = get(id);
    node_id next_id = node.next();
    node.set_next(NULL_NODE);

    formStr(id, output, tabs);

    node.set_next(next_id);

    std::cout << output << std::endl;
}

void TGDB::save(const std::string& filename) const
{
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    if (!file)
        throw std::runtime_error("Cannot open file for writing");

    size_t count = nodes.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    file.write(reinterpret_cast<const char*>(nodes.data()), count * sizeof(Node));
}

void TGDB::load(const std::string& filename) 
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
        throw std::runtime_error("Cannot open file for reading");

    size_t count = 0;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    if (!file)
        throw std::runtime_error("Invalid file format");

    nodes.clear();
    nodes.resize(count);
    file.read(reinterpret_cast<char*>(nodes.data()), count * sizeof(Node));
    if (!file)
        throw std::runtime_error("Error reading node data, maybe file malfunctioned");
}