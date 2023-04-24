#include "Pickup.h"
#include <iostream>

void PickupSystem::on_tick()
{
	for (auto &&[player_entity, player, player_pos] : QueryPlayers::access_storage().each())
	{
		std::vector<entt::entity> to_remove;
		for (auto &&[pickup_entity, pickup, pickup_pos] : QueryPickups::access_storage().each())
		{
			if (abs(player_pos.xy.x - pickup_pos.xy.x) <= pickup.radius &&
			    abs(player_pos.xy.y - pickup_pos.xy.y) <= pickup.radius)
			{
				replace_component<Item>(player_entity, pickup.name, pickup.uses); // Removing this line, in addition to the despawn line will stop the crashing issue.
				to_remove.push_back(pickup_entity);
			}
		}

		for (auto e : to_remove)
		{
			despawn(e);
		}
	}
}

Bool PickupSystem::on_start()
{
	MutAccessUnique<PickupTimer>::access_unique().start();
	return true;
}

void PickupSystem::process_signal(PickupTimeoutSignal& signal)
{
	spawn()
		.with<Pickup>("fastball" , 32.0f, 10)
		.with<Sprite>(ecs::no_entity)
		.with<SpriteAnimation>(Spritesheet::get_by_name("test/pickup"))
		.with<Position>(geometry::Vec2{ rand() % 600, rand() % 570 })
		.with<Visibility>(true);
	spawn()
		.with<Pickup>("curseball" , 32.0f, 3)
		.with<Sprite>(ecs::no_entity)
		.with<SpriteAnimation>(Spritesheet::get_by_name("test/pickup"))
		.with<Position>(geometry::Vec2{ rand() % 600, rand() % 570 })
		.with<Visibility>(true);
}
