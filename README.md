# TGDB — True Graph Database

TGDB is a minimalistic graph database implemented as a static C++17 library.  
All data is stored as fixed-size, uniform nodes connected by index-based pointers.  
The database is schema-less, supports basic types (`int`, `double`, `std::string`), and allows building objects with properties.

## Features

- **Uniform node type** — 56 bytes, 5 pointer fields (`parent`, `child`, `prev`, `next`, `name`) and one 64‑bit data field.
- **Built‑in types** — integers, floating‑point numbers, strings.
- **Bidirectional links** — from parent to child and back.
- **Disk persistence via mmap** — the entire database lives in a memory‑mapped file; use the `sync()` method for explicit disk synchronisation.
- **Template‑based interface** — `db.create<int>(42)`, `db.get<std::string>(id)`.
- **Fast local traversal** — recursive descent from any node following `child` links.
- **Global search** — `first_by_name` / `all_by_name` to scan the whole graph when needed.

## Requirements

- C++17 compiler (GCC ≥ 8, Clang ≥ 7, MSVC ≥ 2017 15.7)
- CMake ≥ 3.14
- [mio](https://github.com/vimpunk/mio) (header‑only, included as a submodule)

## Quick Start

### Building the library

```bash
git clone --recurse-submodules https://github.com/LincolnCox29/TrueGraphDataBase
cd TrueGraphDataBase
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

Alternatively, you can use the `build.bat` script on Windows.

After building, the static library `libtgdb.a` (Linux/macOS) or `tgdb.lib` (Windows) will be in the `build/` directory.

### Example

```cpp
#include <tgdb.h>
#include <iostream>

int main() {
    // Open or create a database file (automatically mmap‑ed)
    tgdb::TGDB db("my_data.tgdb", 1024 * 1024);  // 1M initial capacity

    // Create values
    auto int_id   = db.create<int>(42);
    auto float_id = db.create<double>(3.14);
    auto str_id   = db.create<std::string>("Hello, world!");

    // Read back
    std::cout << db.get<int>(int_id) << std::endl;          // 42
    std::cout << db.get<double>(float_id) << std::endl;     // 3.14
    std::cout << db.get<std::string>(str_id) << std::endl;  // "Hello, world!"

    // Create an object
    auto obj = db.create_object("Person");
    db.add_property(obj, "name", db.create<std::string>("Alice"));
    db.add_property(obj, "age", db.create<int>(30));

    // Retrieve properties
    auto name_id = db.get_property(obj, "name");
    auto age_id  = db.get_property(obj, "age");
    std::cout << "Name: " << db.get<std::string>(name_id)
              << ", Age: " << db.get<int>(age_id) << std::endl;

    // Search for nodes by name (global scan)
    node_id found = db.first_by_name("Alice");
    if (found != tgdb::NULL_NODE)
        std::cout << "Found node with name Alice" << std::endl;

    // Local traversal from a specific node (fast, follows child links only)
    auto results = db.all_by_name(obj, "age");  // locates the property "age" inside obj
    for (auto id : results)
        std::cout << "Property 'age' node ID: " << id << std::endl;

    // Explicitly flush changes to disk (can be called periodically)
    db.sync();

    return 0;
}
```

## Main API

### `TGDB` (constructor & persistence)

- `TGDB(const std::string& path, uint64_t initial_capacity)` – opens an existing database file or creates a new one with the given capacity (in number of nodes). The file is memory‑mapped for direct access.
- `~TGDB()` – automatically calls `sync()` and unmaps the file.
- `void sync()` – explicitly synchronises all changes to disk.

### Creating and reading values

- `node_id create<T>(const T& value)` – create a value of type `int`, `double`, or `std::string`.
- `T get<T>(node_id id) const` – read a value by its ID.
- `void delete_node(node_id)` – delete a node by its ID.  

### Objects and properties

- `node_id create_object(const std::string& type_name)` – create an object with the given type name (the node itself is named `type_name`).
- `void add_property(node_id obj, const std::string& key, node_id prop_value)` – add a named property to an object. The property becomes a separate node linked to the object.
- `node_id get_property(node_id obj, const std::string& key)` – get the value node of a property by key.

### Node naming

- `std::string node_name(node_id id)` – get the node's name (if any).
- `void set_node_name(node_id id, const std::string& name)` – set the node's name (creates a string value internally).

### Search & traversal

- `node_id first_by_name(const std::string& name)` – global linear scan, returns the first node with the given name.
- `node_id first_by_name(node_id start, const std::string& name)` – depth‑first traversal from `start` following `child` links, returns the first matching node.
- `std::vector<node_id> all_by_name(const std::string& name)` – global scan, returns all nodes with that name.
- `std::vector<node_id> all_by_name(node_id start, const std::string& name)` – local traversal from `start` following `child` links, returns all matching nodes.

### Direct node access (advanced)

- `Node& get(node_id id)` / `const Node& get(node_id id) const` – direct access to a node by its index.
- `size_t size() const` – returns the total number of allocated nodes (including the reserved null node).

### Data Types

| C++ Type       | Internal Representation |
|----------------|-------------------------|
| `int`          | 64‑bit integer          |
| `double`       | 64‑bit floating point   |
| `std::string`  | chain of character nodes|
| `node_id`      | reference to another node |

## License

This project is distributed under the **MIT License**. See the `LICENSE` file for the full text.

## Author

- **LincolnCox29** – [GitHub](https://github.com/LincolnCox29)