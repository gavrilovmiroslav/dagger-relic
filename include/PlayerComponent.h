#pragma once

#include "Prelude.h"

#define INITIAL_VALUE 3

enum SpecialBlindfold
{
	HumanEyes = 0,
	FoxEyes = 1,
	ScorpionEyes = 2
};

struct Player
{
	Bool is_pushing;
	Bool all_platforms_checked;
	U32 number_of_platforms_checked;
	SpecialBlindfold current_blindfold;
	Map<SpecialBlindfold, U32> available_blindfolds;

	Player(SpecialBlindfold blindfold, Map<SpecialBlindfold, U32> blindfolds)
		: is_pushing(false)
		, all_platforms_checked(false)
		, number_of_platforms_checked(0u)
		, current_blindfold(blindfold)
		, available_blindfolds(blindfolds)
	{
		// Consider adding random selection of blindfolds at the beginning (instead of setting it on INITIAL_VALUE for each)
	}
};