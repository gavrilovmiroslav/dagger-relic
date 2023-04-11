#pragma once

#include "Prelude.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SPEED_MOD 350.0f
#define NUMBER_OF_BACKGROUND 10
#define NUMBER_OF_PLATFORM 13
#define GRAVITY 200.0f

//bool playerMove = false;

struct Player 
{
	F32 verticalVelocity;
};
struct Background 
{
	F32 verticalVelocity;
};
struct Platform
{
	F32 verticalVelocity;
};

struct OurGlobalVar{
	bool canPlayerMove = false;
	bool isGrounded=false;
};
struct KeyBindings 
{
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
	KeyCode attack;
};


