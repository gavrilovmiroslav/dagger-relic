#pragma once

#include "GameComponents.h"
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

// code sucks, but works fine for now... (SPACE to go on next blindfold, LSHIFT + SPACE to return to previous)
struct BlindfoldChangingSystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, KeyBinding>
	, public MutAccessGroupStorage<Wall, SpriteAnimation>
{
	using QueryPlayer = MutAccessGroupStorage<Player, KeyBinding>;
	using QueryWalls = MutAccessGroupStorage<Wall, SpriteAnimation>;

	U32 counter = 0;
	StaticArray<SpecialBlindfold, 3> blindfolds = { SpecialBlindfold::HumanEyes, SpecialBlindfold::FoxEyes, SpecialBlindfold::FoxEyes };
	SpecialBlindfold new_blindfold;

	void on_tick() override
	{
		const auto& keys = KeyState::get();

		for (auto&& [player_entity, player, key_binding] : QueryPlayer::access_storage().each())
		{
			if (keys.is_pressed(key_binding.blindfold_change))
			{
				counter++;
			}
			else if (keys.is_down(key_binding.reversed_change) && keys.is_pressed(key_binding.blindfold_change))
			{
				counter--;
			}

			new_blindfold = blindfolds[(counter + 3) % 3];

			if (new_blindfold != SpecialBlindfold::HumanEyes && player.available_blindfolds[new_blindfold] != 0)
			{
				player.current_blindfold = new_blindfold;
			}
			else
			{
				player.current_blindfold = SpecialBlindfold::HumanEyes;
			}

			for (auto&& [wall_entity, wall, sprite_animation] : QueryWalls::access_storage().each())
			{
				if (player.current_blindfold == SpecialBlindfold::HumanEyes)
				{
					sprite_animation.change_to("pyramidplunder/sand");
				}
				else if (player.current_blindfold != SpecialBlindfold::HumanEyes)
				{
					if (wall.is_door)
					{
						sprite_animation.change_to("pyramidplunder/door");
					}
					else
					{
						sprite_animation.change_to("pyramidplunder/wall");
					}
				}
			}
		}
	}
};
