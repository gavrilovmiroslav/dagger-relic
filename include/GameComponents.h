#pragma once

#include "Prelude.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 16
#define SPEED_MOD 200.0f
#define NUMBER_OF_BACKGROUND 10
#define NUMBER_OF_PLATFORM 13

//bool playerMove = false;

struct Player {};
struct Background {};
struct Platform{};

struct OurGlobalVar{
	bool canPlayerMove = false;
};
struct KeyBindings 
{
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
	KeyCode attack;
};


