#include "tgdb.h"
#define TGDB_CHUNK_SIZE 8

void TGDB::pack_char_chunk(node_id str_id, const std::string& str)
{
    size_t size = str.size();
    get(str_id).set_raw_value(size);

    size_t chunk_count = (size + TGDB_CHUNK_SIZE - 1) / TGDB_CHUNK_SIZE;
    if (chunk_count == 0) return;

    node_id prev = NULL_NODE;
    node_id first = NULL_NODE;

    for (size_t i = 0; i < chunk_count; ++i)
    {
        node_id chunk = alloc(Type::INT);

        if (prev != NULL_NODE)
            get(prev).set_next(chunk);
        else
            first = chunk;

        uint64_t blob = 0;
        size_t offset = i * TGDB_CHUNK_SIZE;
        size_t len = std::min(static_cast<size_t>(TGDB_CHUNK_SIZE), size - offset);
        memcpy(&blob, str.data() + offset, len);
        get(chunk).set_raw_value(blob);

        prev = chunk;
    }

    get(str_id).set_child(first);
}

std::string TGDB::unpack_char_chunk(node_id str_id) const
{
    const Node& str_node = get(str_id);
    size_t size = str_node.raw_value();
    std::string out;

    node_id cur = str_node.child();
    while (cur != NULL_NODE)
    {
        uint64_t blob = get(cur).raw_value();
        for (int c = 0; c < TGDB_CHUNK_SIZE; ++c)
            out.push_back(static_cast<char>((blob >> (c * 8)) & 0xFF));
        cur = get(cur).next();
    }

    out.resize(size);
    return out;
}

std::string TGDB::read_string(node_id id) const
{
    const Node& n = get(id);
    if (n.type() != Type::STRING)
        throw std::runtime_error("Not a string");
    return unpack_char_chunk(id);
}

template<>
node_id TGDB::create<std::string>(const std::string& value)
{
    node_id str_id = alloc(Type::STRING);
    if (value.empty())
        return str_id;

    pack_char_chunk(str_id, value);

    return str_id;
}