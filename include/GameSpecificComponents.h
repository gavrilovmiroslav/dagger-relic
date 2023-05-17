#pragma once

#include "Prelude.h"
#include "Timer.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SPEED_MOD 300.0f
#define GRAVITY_MOD 400.0f
#define JUMP_MOD 400.0f

#include "PlayerFSM.h"

struct Player
{
	PlayerFSM fsm;
	F32 vertical_velocity;
};

struct PlayerId
{
	ecs::Entity playerId;
};

struct Platform
{
	F32 width;
	F32 height;
};

struct Background
{};

struct Pickup
{
	String name;
	F64 radius;
	I32 uses;
};

struct Item
{
	String name;
	I32 uses;
};

struct Spell
{
	F32 speed;
	F32 start_pos;
	F32 range;
	F32 radius;
	I32 damage;
};

struct Status
{
	I32 health;
	String win_text;
};

struct HealthBar
{
	I32 health;
	I32 dashes;

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

struct HealthUpdateSignal
{
	ecs::Entity player;
	I32 update;
};

struct OnStartSignal
{
	I32 start_health;
};

struct PickupSpawnSignal
{};

struct KeyBindings
{
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
	KeyCode basic_attack;
	KeyCode special_attack;
};
