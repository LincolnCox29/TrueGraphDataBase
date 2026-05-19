#include "tgdb.h"

void TGDB::dealloc(node_id id)
{
    if (id == NULL_NODE) return;
    Node& node = get(id);

    if (node.name() != NULL_NODE)
    {
        dealloc(node.name());
    }

    if (node.child() != NULL_NODE)
    {
        node_id cur = node.child();
        while (cur != NULL_NODE)
        {
            node_id next = get(cur).next();
            dealloc(cur);
            cur = next;
        }
    }

    node.set_child(NULL_NODE);
    node.set_parent(NULL_NODE);
    node.set_next(NULL_NODE);
    node.set_prev(NULL_NODE);
    node.set_name(NULL_NODE);
    node.set_type(Type::DELETED);
    node.set_raw_value(0);

    Node& dealloc_seq = get(dealloc_sequence_id);
    node_id old_head = dealloc_seq.child();
    node.set_next(old_head);
    dealloc_seq.set_child(id);
}

void TGDB::delete_node(node_id id)
{
    if (id == NULL_NODE) return;
    Node& node = get(id);

    node_id parent_id = node.parent();
    node_id prev_id = node.prev();
    node_id next_id = node.next();

    if (prev_id != NULL_NODE) get(prev_id).set_next(next_id);
    if (next_id != NULL_NODE) get(next_id).set_prev(prev_id);
    if (parent_id != NULL_NODE)
    {
        Node& parent = get(parent_id);
        if (parent.child() == id) parent.set_child(next_id);
    }

    dealloc(id);
}

node_id TGDB::pop_dealloc_seq()
{
    Node& dealloc_seq = get(dealloc_sequence_id);
    node_id top_id = dealloc_seq.child();
    if (top_id == NULL_NODE) return NULL_NODE;
    Node& top = get(top_id);
    node_id next_id = top.next();

#ifdef DEBUG
    std::cout << "Reuse this shit! :\n";
    print_node(dealloc_sequence_id);
#endif // DEBUG

    dealloc_seq.set_child(next_id);
    top.set_next(NULL_NODE);
    return top_id;
}