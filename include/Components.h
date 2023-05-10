#pragma once

#include "Prelude.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 16
#define SPEED_MOD 200.0f

using namespace core;

struct Platform {
	F32 width;
	F32 height;
};

struct Player {
    geometry::Vec2 speed;
	bool isGrounded;
	F32 visina;
};

struct Box {
	geometry::Vec2 speed;
	bool isGrounded;
};

struct KeyBindings {
	KeyCode jump;
	KeyCode left;
	KeyCode right;
};

struct Gravity {
	geometry::Vec2 acceleration;
};

struct Animation{
	String jump;
	String walk;
	String idle;
};