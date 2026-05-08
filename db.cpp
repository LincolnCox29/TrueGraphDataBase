#include "db.h"
#include <string>
#include <iostream>
#include <functional>

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

node_id TGDB::create_string_internal(const std::string& s) {
    node_id str_id = alloc(Type::STRING);
    if (s.empty()) return str_id;

    std::vector<node_id> chars;
    for (char c : s) {
        node_id char_id = alloc(Type::CHAR);
        get(char_id).set_raw_value(static_cast<uint8_t>(c));
        chars.push_back(char_id);
    }
    for (size_t i = 0; i < chars.size() - 1; ++i)
        get(chars[i]).set_next(chars[i + 1]);

    get(str_id).set_child(chars[0]);
    return str_id;
}

std::string TGDB::read_string(node_id id) const {
    const Node& n = get(id);
    if (n.type() != Type::STRING)
        throw std::runtime_error("Not a string");
    std::string result;
    node_id cur = n.child();
    while (cur != 0) {
        const Node& c = get(cur);
        result.push_back(static_cast<char>(c.raw_value()));
        cur = c.next();
    }
    return result;
}

node_id TGDB::alloc(Type t) {
    node_id id = nodes.size();
    nodes.emplace_back();
    nodes.back().set_type(t);
    return id;
}

TGDB::TGDB() {
    nodes.emplace_back();
}

template<>
node_id TGDB::create<int>(const int& value) {
    node_id id = alloc(Type::INT);
    get(id).set_raw_value(static_cast<uint64_t>(value));
    return id;
}

template<>
node_id TGDB::create<double>(const double& value) {
    node_id id = alloc(Type::FLOAT);
    uint64_t raw;
    std::memcpy(&raw, &value, sizeof(raw));
    get(id).set_raw_value(raw);
    return id;
}

template<>
node_id TGDB::create<std::string>(const std::string& value) {
    return create_string_internal(value);
}

template<>
int TGDB::get<int>(node_id id) const {
    const Node& n = get(id);
    if (n.type() != Type::INT)
        throw std::runtime_error("Not an integer");
    return static_cast<int>(n.raw_value());
}

template<>
double TGDB::get<double>(node_id id) const {
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
    node_id obj = alloc(Type::VOID);
    set_node_name(obj, type_name);
    return obj;
}

void TGDB::add_property(node_id obj, const std::string& key, node_id value_id) 
{
    node_id prop = alloc(Type::VOID);
    set_node_name(prop, key);
    Node& p = get(prop);
    p.set_parent(obj);
    p.set_child(value_id);
    Node& obj_node = get(obj);
    p.set_next(obj_node.child());
    obj_node.set_child(prop);
}

node_id TGDB::get_property(node_id obj, const std::string& key)
{
    node_id cur = get(obj).child();
    while (cur != 0) {
        const Node& prop = get(cur);
        if (prop.type() == Type::VOID && node_name(cur) == key)
            return prop.child();
        cur = prop.next();
    }
    return 0;
}

void TGDB::print_node(node_id id)
{
    std::string output;
    int tabs = 0;

    std::function<void(node_id, std::string&, int&)> formStr;
    formStr = [&](node_id cur_id, std::string& out, int& t)
    {
        node_id cur = cur_id;
        Type type = get(cur_id).type();

        while (cur != NULL_NODE)
        {
            if (type == Type::VOID)
            {
                for (int i = 0; i < t; ++i) out += "\t";
                out += node_name(cur) + "\n";
            }

            node_id child = get(cur).child();
            if (child != NULL_NODE)
            {
                ++t;
                formStr(child, out, t);
                --t;
            }
            cur = get(cur).next();
        }
    };

    output += node_name(id) + "\n";
    ++tabs;
    node_id child = get(id).child();
    if (child != NULL_NODE) formStr(child, output, tabs);
    std::cout << output << std::endl;
}