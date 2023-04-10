#pragma once

#include "Prelude.h"
#include "Shuffle.h"

enum class SpecialBlindfold
{
	HumanEyes,
	FoxEyes,
	ScorpionEyes
};

struct Constraints
{
	U32 number_of_blindfolds = 6;
	U32 max_number_of_blindfolds = 20;
};

struct Player
{
	Bool is_pushing;
	SpecialBlindfold current_blindfold;
	Map<SpecialBlindfold, U32> available_blindfolds;

	Player(SpecialBlindfold blindfold)
		: is_pushing(false)
		, current_blindfold(blindfold)
	{
		DynamicArray<SpecialBlindfold> blindfolds = {};

		Constraints constraint;
		blindfolds.insert(blindfolds.begin(), constraint.max_number_of_blindfolds, SpecialBlindfold::FoxEyes);
		blindfolds.insert(blindfolds.begin(), constraint.max_number_of_blindfolds, SpecialBlindfold::ScorpionEyes);

		randomize(blindfolds);
		for (U32 i = 0; i < constraint.number_of_blindfolds; ++i)
		{
			available_blindfolds[blindfolds[i]]++;
		}
	}
};