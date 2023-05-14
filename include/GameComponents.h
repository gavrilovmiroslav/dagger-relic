#pragma once

#include "Prelude.h"

enum class ButtonType
{
	PauseMusic,
	PlayNext,
	PlayPrevious
};

struct Blindfold {};
struct Boulder {};

struct Box
{
	I32 id;
};


struct Button
{
	ButtonType type;
};

struct KeyBinding
{
	KeyCode left, down, up, right;
	KeyCode blindfold_change, reversed_change;
};

struct PushPlateActivatedSignal {};

struct Trap {};

struct Wall
{
	Bool is_door = false;
};