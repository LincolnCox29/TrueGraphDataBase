#include "tgdb.h"

template<>
node_id TGDB::create<node_id>(const node_id& target)
{
    node_id id = alloc(Type::REF);
    get(id).set_raw_value(target);
    return id;
}

template<>
node_id TGDB::get<node_id>(node_id id) const
{
    const Node& n = get(id);
    if (n.type() != Type::REF)
        throw std::runtime_error("Not a reference");
    return static_cast<node_id>(n.raw_value());
}
