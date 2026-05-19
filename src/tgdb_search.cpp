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

node_id TGDB::first_by_name(node_id start, const std::string& name)
{
    node_id res = NULL_NODE;
    std::function<void(node_id)> recSearch;
    recSearch = [&](node_id start_id)
    {
        node_id cur_id = start_id;
        while (cur_id != NULL_NODE)
        {
            if (res != NULL_NODE) return;

            Node& cur = get(cur_id);

            if (node_name(cur_id) == name && !is_dealloced(cur_id))
            {
                res = cur_id;
                return;
            }

            node_id child = cur.child();
            if (child != NULL_NODE)
            {
                recSearch(child);
            }

            cur_id = cur.next();
        }
    };

     recSearch(start);

     return res;
}

node_id TGDB::first_by_name(const std::string& name)
{
    for (node_id id = 1; id < next_id_; ++id)
    {
        if (node_name(id) == name && !is_dealloced(id))
            return id;
    }
    return NULL_NODE;
}

std::vector<node_id> TGDB::all_by_name(node_id start, const std::string& name)
{
    std::vector<node_id> res;
    std::function<void(node_id)> recSearch;
    recSearch = [&](node_id start_id)
    {
        node_id cur_id = start_id;
        while (cur_id != NULL_NODE)
        {
            Node& cur = get(cur_id);

            if (node_name(cur_id) == name && !is_dealloced(cur_id))
            {
                res.push_back(cur_id);
            }

            node_id child = cur.child();
            if (child != NULL_NODE)
            {
                recSearch(child);
            }

            cur_id = cur.next();
        }
    };

    recSearch(start);

    return res;
}

std::vector<node_id> TGDB::all_by_name(const std::string& name)
{
    std::vector<node_id> result;
    for (node_id id = 1; id < next_id_; ++id) 
    {
        if (node_name(id) == name && !is_dealloced(id))
            result.push_back(id);
    }
    return result;
}