#pragma once

#include "Prelude.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 16
#define SPEED_MOD 200.0f

using namespace core;

struct Player {
    geometry::Vec2 speed;
};

struct KeyBindings {
	KeyCode jump;
	KeyCode left;
	KeyCode right;
};

struct Gravity {
	geometry::Vec2 acceleration;
};