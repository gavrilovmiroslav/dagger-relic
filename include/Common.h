#pragma once

#include "Definitions.h"
#include <string>

struct Player
{
};

struct Enemy
{
	geometry::Vec2 speed;
};

struct Damage
{
	ecs::Entity from;
	ecs::Entity to;
	I32 damage;
};

struct Health
{
	I32 health;
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