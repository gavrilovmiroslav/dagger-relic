#include "Player.h"

Player::Player(SpecialBlindfold blindfold)
	: current_blindfold(blindfold)
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

void BlindfoldChangingSystem::on_tick()
{
	const auto& keys = KeyState::get();

	Bool walls_visible = AccessComponentById<Visibility>::get(QueryWalls::access_storage().front()).state;
	Bool boxes_visible = AccessComponentById<Visibility>::get(QueryBoxes::access_storage().front()).state;

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

		if (player.current_blindfold == SpecialBlindfold::HumanEyes)
		{
			if (!walls_visible)
			{
				for (auto&& [wall_entity, wall, visibility] : QueryWalls::access_storage().each())
				{
					visibility.state = true;
				}
			}

			if (boxes_visible)
			{
				for (auto&& [box_entity, visibility] : QueryBoxes::access_storage().each())
				{
					visibility.state = false;
				}
			}
		}
		else if (player.current_blindfold == SpecialBlindfold::FoxEyes)
		{
			if (walls_visible)
			{
				for (auto&& [wall_entity, wall, visibility] : QueryWalls::access_storage().each())
				{
					visibility.state = false;
				}
			}

			if (!boxes_visible)
			{
				for (auto&& [box_entity, visibility] : QueryBoxes::access_storage().each())
				{
					visibility.state = true;
				}
			}
		}
		else if (player.current_blindfold == SpecialBlindfold::ScorpionEyes)
		{
			if (!walls_visible)
			{
				for (auto&& [wall_entity, wall, visibility] : QueryWalls::access_storage().each())
				{
					visibility.state = true;
				}
			}

			if (!boxes_visible)
			{
				for (auto&& [box_entity, visibility] : QueryBoxes::access_storage().each())
				{
					visibility.state = true;
				}
			}
		}
	}
}
