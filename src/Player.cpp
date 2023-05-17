#include "Player.h"
#include "Scene.h"

Player::Player(SpecialBlindfold blindfold)
	: current_blindfold(blindfold)
{

	DynamicArray<SpecialBlindfold> blindfolds = {};

	blindfolds.insert(blindfolds.begin(), max_number_of_blindfolds, SpecialBlindfold::FoxEyes);
	blindfolds.insert(blindfolds.begin(), max_number_of_blindfolds, SpecialBlindfold::ScorpionEyes);

	randomize(blindfolds);
	for (U32 i = 0; i < number_of_blindfolds; ++i)
	{
		available_blindfolds[blindfolds[i]]++;
	}
}

void BlindfoldChangingSystem::on_tick()
{
	if(!scene.in_game)
	{
		return;
	}

	const auto& keys = KeyState::get();

	Bool walls_visible = AccessComponentById<Visibility>::get(QueryWalls::access_storage().front()).state;
	Bool boxes_visible = AccessComponentById<Visibility>::get(QueryBoxes::access_storage().front()).state;
	Bool boulders_visible = AccessComponentById<Visibility>::get(QueryBoulders::access_storage().front()).state;

	for (auto&& [player_entity, player, key_binding] : QueryPlayer::access_storage().each())
	{
		if (keys.is_down(key_binding.reversed_change) && keys.is_pressed(key_binding.blindfold_change))
		{
			counter--;
		}
		else if (keys.is_pressed(key_binding.blindfold_change))
		{
			counter++;
		}

		new_blindfold = blindfolds[(counter + NUM_OF_BLINDFOLDS) % NUM_OF_BLINDFOLDS];

		player.current_blindfold = player.available_blindfolds[new_blindfold] != 0 ? new_blindfold : SpecialBlindfold::HumanEyes;

		switch (player.current_blindfold)
		{
			case SpecialBlindfold::HumanEyes:
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
					for (auto&& [box_entity, box, visibility] : QueryBoxes::access_storage().each())
					{
						visibility.state = false;
					}
				}

				if (!boulders_visible)
				{
					for (auto&& [boulder_entity, visibility] : QueryBoulders::access_storage().each())
					{
						visibility.state = true;
					}
				}
			} break;
			case SpecialBlindfold::FoxEyes:
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
					for (auto&& [box_entity, box, visibility] : QueryBoxes::access_storage().each())
					{
						visibility.state = true;
					}
				}

				if (!boulders_visible)
				{
					for (auto&& [boulder_entity, visibility] : QueryBoulders::access_storage().each())
					{
						visibility.state = true;
					}
				}
			} break;
			case SpecialBlindfold::ScorpionEyes:
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
					for (auto&& [box_entity, box, visibility] : QueryBoxes::access_storage().each())
					{
						visibility.state = true;
					}
				}

				if (boulders_visible)
				{
					for (auto&& [boulder_entity, visibility] : QueryBoulders::access_storage().each())
					{
						visibility.state = false;
					}
				}
			} break;
		}
	}
}
