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

struct Spell
{
	F32 speed;
	F32 range;
	F32 radius;
};

enum Element // Spell element
{
	Fire = 0,
	Wind = 1,
	Earth = 2,
	Water = 3
};

enum Shape // Currently either one spell or a spray of them
{
	Ray = 0,
	Spray = 1
};

enum Move // If the spell moves, and the type of movement
{
	Still = 0,
	Line = 1
};

enum Duration // If the spell disappears after hitting a target or lingers on
{
	Disappear = 0,
	Lingers = 1
};

enum Clipping // If the spell collides with terrain
{
	Pass = 0,
	Collides = 1
};

struct SpellPlatformCollisionSignal
{
	ecs::Entity platform;
	ecs::Entity spell;
};

struct SpellPlayerCollisionSignal
{
	ecs::Entity player;
	ecs::Entity spell;
};

struct KeyBindings 
{
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
	KeyCode basic_attack;
	KeyCode special_attack;
};