#pragma once

#include "GameComponents.h"
#include "Prelude.h"
#include "Shuffle.h"

#define NUM_OF_BLINDFOLDS 3

enum class SpecialBlindfold
{
	HumanEyes,
	FoxEyes,
	ScorpionEyes
};

#define number_of_blindfolds  6
#define max_number_of_blindfolds 20

struct Player
{
	SpecialBlindfold current_blindfold;
	Map<SpecialBlindfold, U32> available_blindfolds;

	Player(SpecialBlindfold blindfold);
};

// code still sucks, but works fine for now... (SPACE to go on next blindfold, LSHIFT + SPACE to return to previous)
struct BlindfoldChangingSystem
	: public ecs::System
	, public AccessComponentById<Visibility>
	, public MutAccessGroupStorage<Box, Visibility>
	, public MutAccessGroupStorage<Player, KeyBinding>
	, public MutAccessGroupStorage<Boulder, Visibility>
	, public MutAccessGroupStorage<Wall, Visibility>
{
	using QueryPlayer = MutAccessGroupStorage<Player, KeyBinding>;
	using QueryWalls = MutAccessGroupStorage<Wall, Visibility>;
	using QueryBoulders = MutAccessGroupStorage<Boulder, Visibility>;
	using QueryBoxes = MutAccessGroupStorage<Box, Visibility>;

	// Bool traps_visible = AccessComponentById<Visibility>::get(QueryTraps::access_storage().front()).state;

	U32 counter = 0;
	StaticArray<SpecialBlindfold, 3> blindfolds = { SpecialBlindfold::HumanEyes, SpecialBlindfold::FoxEyes, SpecialBlindfold::ScorpionEyes };
	SpecialBlindfold new_blindfold;

	void on_tick() override;
};
