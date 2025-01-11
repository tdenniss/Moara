#pragma once
#include <stdint.h>

enum class EGameState : uint8_t
{
    Placing,
    Moving,
    Removing,
    Finished
};
