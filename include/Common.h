#pragma once

#include "Definitions.h"
#include <string>

struct Player
{
	F32 health;
};

struct Enemy
{
	geometry::Vec2 speed;
	F32 health;
};

struct Healthbar
{
    F32 value;
};

struct KeyBindings
{
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
	KeyCode attack;
};