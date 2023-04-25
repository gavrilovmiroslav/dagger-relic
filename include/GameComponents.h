#pragma once

#include "Prelude.h"

struct KeyBinding
{
	KeyCode left, down, up, right;
	KeyCode toggle_sound;

	KeyBinding(KeyCode left, KeyCode down, KeyCode up, KeyCode right)
		: left(left)
		, down(down)
		, up(up)
		, right(right)
		{};

	KeyBinding(KeyCode toggle)
		: toggle_sound(toggle)
		{};
};

struct Movement
{
	geometry::Vec2 velocity;
	geometry::Vec2 velocity_max;
	geometry::Vec2 force;
	F32            move_force;
	F32            collision_box_width;
	F32            collision_box_height;

	Movement(F32 move_force, F32 velocity_max, F32 collision_w, F32 collision_h)
		: velocity(0.0f, 0.0f)
		, move_force(move_force)
		, velocity_max(velocity_max)
		, force(0.0f, 0.0f)
		, collision_box_width(collision_w)
		, collision_box_height(collision_h)
	{

	}
};

struct Wall {};
struct Boulder {};
