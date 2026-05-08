# TGDB — True Graph Database

TGDB is a minimalistic graph database implemented as a static C++17 library.  
All data is stored as fixed-size, uniform nodes connected by index-based pointers.  
The database is schema-less, supports basic types (`int`, `double`, `std::string`), and allows building objects with properties.

## Features

- **Uniform node type** — 56 bytes, 5 pointer fields (`parent`, `child`, `prev`, `next`, `name`) and one 64‑bit data field.
- **Built‑in types** — integers, floating‑point numbers, strings.
- **Bidirectional links** — from parent to child and back.
- **Persistence** — built‑in `save()` and `load()` methods for binary snapshots.
- **Template‑based interface** — `db.create<int>(42)`, `db.get<std::string>(id)`.

## Requirements

- C++17 compiler (GCC ≥ 8, Clang ≥ 7, MSVC ≥ 2017 15.7)
- CMake ≥ 3.14

## Quick Start

### Building the library

```bash
git clone https://github.com/LincolnCox29/TrueGraphDataBase
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
    tgdb::TGDB db;

    // Create values of different types
    auto int_id   = db.create<int>(42);
    auto float_id = db.create<double>(3.14);
    auto str_id   = db.create<std::string>("Hello, world!");

    // Read values
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

    // Save the database to a file
    db.save("my_db.tgdb");

    // Load from a file
    TGDB db2;
    db2.load("my_db.tgdb");

    return 0;
}
```

## Main API

### `TGDB`

- `node_id create<T>(const T& value)` – create a value of type `int`, `double`, or `std::string`.
- `T get<T>(node_id id) const` – read a value by its ID.
- `node_id create_object(const std::string& type_name)` – create an object with the given type name.
- `void add_property(node_id obj, const std::string& key, node_id value)` – add a property to an object.
- `node_id get_property(node_id obj, const std::string& key)` – get the value of a property.
- `void save(const std::string& filename) const` – save the entire database to a binary file.
- `void load(const std::string& filename)` – load the database from a binary file.
- `std::string node_name(node_id id)` – get the node's name (if any).
- `void set_node_name(node_id id, const std::string& name)` – set the node's name.

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
