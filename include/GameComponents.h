#pragma once

#include "Prelude.h"

#include "SDL_ttf.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SPEED_MOD 350.0f
#define NUMBER_OF_BACKGROUND 20
#define NUMBER_OF_PLATFORM 13
#define GRAVITY 200.0f
#define PLAYER_OFFSET_LEFT 35
#define PLAYER_OFFSET_RIGHT 70
#define PLAYER_OFFSET_DOWN 142
#define MONSTER_OFFSET_LEFT 35
#define MONSTER_OFFSET_RIGHT 100
#define PLATFORM_OFFSET 64
// bool playerMove = false;

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
struct Item
{
	F32 verticalVelocity;
	F64 pickupRadius;
};
struct OurGlobalVar
{
	bool canPlayerMove = false;
	bool isGrounded = false;
	bool shouldDespawn = false;
	I32 coinNumber = 0;
	int timeMs = 0;
	bool didPlayerWon = true;
	F32 playerHealth = 70.0;
};
struct KeyBindings
{
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
	KeyCode attack;
};
struct Monster
{
	F32 verticalVelocity;
};
struct CharacterId
{
	ecs::Entity characterId;
};
struct Status
{
	F32 health;
};
struct HealthBar
{
	F32 health;
	I32 dashes;

};
struct HealthUpdateSignal
{
	ecs::Entity player;
	I32 update;
};
struct PlayerCollisionSignal
{
	ecs::Entity enemy;
	ecs::Entity player;
};

struct OnStartSignal
{
	I32 start_health;
};

struct ScoreRender
{
	TTF_Font* font;
	String text;
	U32 font_size;
	SDL_Color text_color = {233, 245, 2};
	ScoreRender(String text, U32 font_size);

	static void init();
	static void deinit();
};
