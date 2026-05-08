#pragma once
#include <cstdint>

enum class Type : uint8_t
{
    INT    = 1,
    FLOAT  = 2,
    STRING = 3,
    CHAR   = 4,
    STRUCT = 5,
    VOID   = 6
};