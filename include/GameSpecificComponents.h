#pragma once

#include "Prelude.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SPEED_MOD 300.0f
#define GRAVITY_MOD 400.0f
#define JUMP_MOD 600.0f

struct Player {
	bool is_grounded;
	float jump_speed;
	float vertical_velocity;
};

struct Platform {
	float width;
	float height;
};

struct Pickup {
	const char* name = new char(51);
	double radius;
	bool is_picked;
};

struct BasicAttack {
	float speed;
	float range;
	float radius;
};

struct KeyBindings {
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
	KeyCode basic_attack;
};