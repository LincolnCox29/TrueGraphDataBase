#include "db.h"
#include <functional>
#include <iostream>

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

    node.set_child(next_id);

    std::cout << output << std::endl;
}