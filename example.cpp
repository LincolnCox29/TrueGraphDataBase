#include "tgdb.h"
#include "iostream"

TGDB db("./db.tgdb", 64);

node_id create_weapon(std::string name, std::string description, std::string attack_modifier, int damage);
node_id create_player(std::string name, int age, float speed, node_id weapon_id);

// MAIN FUNC FOR EXAMPLE
int main() 
{
    std::cout << "Create player"<< std::endl;

    node_id weapon = create_weapon("halberd", "halberd shining with fire", "burning", 10);
    node_id player = create_player("Lincoln Cox", 18, 0.5, weapon);
    db.print_node(player);

    std::cout << "Deleat weapon" << std::endl;

    db.delete_node(weapon);
    db.print_node(player);

    std::cout << "Add weapon struct again" << std::endl;

    weapon = create_weapon("halberd", "halberd shining with fire", "burning", 10);
    db.add_property(player, "weapon", weapon);
    db.print_node(player);

    std::cout << "Trying to find weapon node by type name" << std::endl;

    weapon = db.first_by_name("weapon");

    db.print_node(weapon);

    return 0;
}

node_id create_player(std::string name, int age, float speed, node_id weapon_id)
{
    node_id player = db.create_object("player");
    db.add_property(player, "name", db.create<std::string>(name));
    db.add_property(player, "age", db.create<int>(age));
    db.add_property(player, "speed", db.create<double>(speed));
    db.add_property(player, "weapon", weapon_id);

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
