#include "tgdb.h"
#include "iostream"

TGDB db("./example_db.tgdb", 64);

node_id create_weapon(std::string name, std::string description, std::string attack_modifier, int damage);
node_id create_player(std::string name, int age, float speed, node_id weapon_id);

// MAIN FUNC FOR EXAMPLE
int main() 
{
    std::cout << "Create player weapon" << std::endl;

    node_id weapon = create_weapon("halberd", "halberd shining with fire", "burning", 10);
    db.print_node(weapon);

    std::cout << "Create players"<< std::endl;

    node_id player1 = create_player("Lincoln Cox", 20, 0.85, weapon);
    node_id player2 = create_player("Goonking", 18, 0.7, weapon);
    db.print_node(player1);
    db.print_node(player2);

    std::cout << "Get weapon node by ref from player node" << std::endl;
    weapon = db.first_by_name(player1, "weapon");
    db.print_node(db.get<node_id>(weapon));

    return 0;
}

node_id create_player(std::string name, int age, float speed, node_id weapon_id)
{
    node_id player = db.create_object("player");
    db.add_property(player, "name", db.create<std::string>(name));
    db.add_property(player, "age", db.create<int>(age));
    db.add_property(player, "speed", db.create<double>(speed));
    db.add_property(player, "weapon", db.create<node_id>(weapon_id));

    return player;
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
