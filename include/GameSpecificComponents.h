#pragma once

#include "Prelude.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SPEED_MOD 300.0f
#define GRAVITY_MOD 400.0f
#define JUMP_MOD 600.0f

struct Player
{
	Bool is_grounded;
	F32 jump_speed;
	F32 vertical_velocity;
};

struct Platform
{
	F32 width;
	F32 height;
};

struct Pickup
{
	LiteralString name = new char(51);
	F64 radius;
	Bool is_picked;
};

struct BasicAttack
{
	F32 speed;
	F32 range;
	F32 radius;
};

struct KeyBindings 
{
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
	KeyCode basic_attack;
};