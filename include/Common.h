#pragma once

#include "Definitions.h"

struct Player
{
	F32 health;
};

struct Enemy
{
	geometry::Vec2 speed;
	F32 health;
};

struct KeyBindings
{
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
	KeyCode attack;
};