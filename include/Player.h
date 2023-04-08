#pragma once

#include "Prelude.h"
#include "Shuffle.h"

#define MAX_NUMBER_OF_BLINDFOLDS 6
#define NUMBER_OF_BLINDFOLDS 20

enum SpecialBlindfold
{
	HumanEyes = 0,
	FoxEyes = 1,
	ScorpionEyes = 2
};

struct Player
{
	Bool is_pushing;
	Bool all_push_plates_checked;
	SpecialBlindfold current_blindfold;
	Map<SpecialBlindfold, U32> available_blindfolds;

	Player(SpecialBlindfold blindfold)
		: is_pushing(false)
		, all_push_plates_checked(false)
		, current_blindfold(blindfold)
	{
		DynamicArray<SpecialBlindfold> blindfolds = {};
		blindfolds.insert(blindfolds.begin(), NUMBER_OF_BLINDFOLDS, FoxEyes);
		blindfolds.insert(blindfolds.begin(), NUMBER_OF_BLINDFOLDS, ScorpionEyes);

		randomize(blindfolds);
		for (U32 i = 0; i < MAX_NUMBER_OF_BLINDFOLDS; ++i)
		{
			available_blindfolds[blindfolds[i]]++;
		}
	}
};