#pragma once

#include "Definitions.h"

struct Player
{
};

struct Enemy
{
	geometry::Vec2 speed;
	F32 movement_speed;
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

struct KeyBindings
{
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
	KeyCode attack;
};