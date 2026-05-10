#include "tgdb.h"
#include "iostream"

TGDB db("./db.tgdb", 64);

node_id create_weapon(std::string name, std::string description, std::string attack_modifier, int damage);

// MAIN FUNC FOR EXAMPLE
int main() 
{
    node_id player = db.create_object("player");
    db.add_property(player, "move_speed", db.create<int>(52));
    db.add_property(player, "player_name", db.create<std::string>("Lincoln Cox"));

    db.add_property(player, "weapon", create_weapon(
        "halberd",
        "halberd shining with fire",
        "burning",
        10
    ));

    db.add_property(player, "weapon", create_weapon(
        "sword",
        "sword with poisonous blade",
        "poisoning",
        7
    ));

    std::cout << "Printing player node\n" << std::endl;
    db.print_node(player);

    std::cout << "Find first weapon node by name\n" << std::endl;
    node_id weapon_node = db.first_by_name("weapon");
    db.print_node(weapon_node);

    std::cout << "Find all weapon nodes by name\n" << std::endl;
    std::vector<node_id> weapon_nodes = db.all_by_name("weapon");
    for (node_id node : weapon_nodes)
        db.print_node(node);

    return 0;
}

node_id create_weapon(std::string name, std::string description, std::string attack_modifier, int damage)
{
    node_id weapon = db.create_object("weapon");

    db.add_property(weapon, "name", db.create<std::string>(name));
    db.add_property(weapon, "description", db.create<std::string>(description));
    db.add_property(weapon, "attack_modifier", db.create<std::string>(attack_modifier));
    db.add_property(weapon, "damage", db.create<int>(damage));

    return weapon;
}
