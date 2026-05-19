cmake_minimum_required(VERSION 3.14)
project(TGDB VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(tgdb STATIC
    src/tgdb.cpp
    src/node.cpp
    src/tgdb_out.cpp
    src/tgdb_dealloc_seq.cpp
    src/tgdb_ref.cpp
    src/tgdb_out.cpp
    src/tgdb_search.cpp
    src/tgdb_string.cpp
)

target_include_directories(tgdb PUBLIC thirdparty/mio/include)
target_include_directories(tgdb PUBLIC thirdparty/mio/single_include)
target_include_directories(tgdb PUBLIC include)
