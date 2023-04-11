#pragma once

#include "Definitions.h"

struct Player
{};

struct Enemy
{
	geometry::Vec2 speed;
};

struct KeyBindings
{
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
	KeyCode attack;
};