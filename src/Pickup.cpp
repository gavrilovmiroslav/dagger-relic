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
				replace_component<Item>(player_entity, pickup.name, pickup.uses);
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
	auto& generator = MutAccessUnique<Generator>::access_unique();
	std::vector<I32> x_coordinates = {100, 200, 300, 400, 500, 600, 700};
	std::vector<I32> y_coordinates = {100, 200, 300, 400, 500, 600, 700};

	MutAccessUnique<PickupTimer>::access_unique().start();
	generator.initialize_deques(x_coordinates, y_coordinates);
	return true;
}

void PickupSystem::process_signal(PickupTimeoutSignal& signal)
{
	auto& generator = MutAccessUnique<Generator>::access_unique();
	I32 x = generator.next_x();
	I32 y = generator.next_y();
	spawn()
		.with<Pickup>("curseball" , 24.0f, 3)
		.with<Sprite>(ecs::no_entity)
		.with<SpriteAnimation>(Spritesheet::get_by_name("test/CurseballTome"))
		.with<Scale>(geometry::Vec2{0.8f, 0.8f})
		.with<Position>(geometry::Vec2{ x, y })
		.with<Visibility>(true);
}
