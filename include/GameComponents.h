#pragma once

#include "Prelude.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SPEED_MOD 350.0f
#define NUMBER_OF_BACKGROUND 20
#define NUMBER_OF_PLATFORM 13
#define GRAVITY 200.0f
#define PLAYER_OFFSET_LEFT 35
#define PLAYER_OFFSET_RIGHT 70
#define PLAYER_OFFSET_DOWN 142
#define PLATFORM_OFFSET 64
// bool playerMove = false;

struct Player
{
	F32 verticalVelocity;
	I32 coinNumber;
};
struct Background
{
	F32 verticalVelocity;
};
struct Platform
{
	F32 verticalVelocity;
};
struct Item
{
	F32 verticalVelocity;
	F64 pickupRadius;
};
struct OurGlobalVar
{
	bool canPlayerMove = false;
	bool isGrounded = false;
};
struct KeyBindings
{
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
	KeyCode attack;
};
