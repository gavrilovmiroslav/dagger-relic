#pragma once

#include "Prelude.h"

struct Rock
{
    Bool is_moving; // once hit, the rock will keep on moving until something makes it stop
    geometry::Vec2 movement_direction;

    Rock()
    : is_moving(false),
      movement_direction(0,0)
    {
    }
};