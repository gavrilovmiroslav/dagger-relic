#include "Pickup.h"
#include <iostream>

void PickupSystem::on_tick()
{
	for (auto &&[player_entity, player, player_pos, status] : QueryPlayers::access_storage().each())
	{
		std::vector<entt::entity> to_remove;
		for (auto &&[pickup_entity, pickup, pickup_pos] : QueryPickups::access_storage().each())
		{
			if (abs(player_pos.xy.x - pickup_pos.xy.x) <= pickup.radius &&
			    abs(player_pos.xy.y - pickup_pos.xy.y) <= pickup.radius)
			{
				if (pickup.name == "healthpotion")
				{
					status.health = std::min(100, status.health + 30);
				}
				else {
					replace_component<Item>(player_entity, pickup.name, pickup.uses);
				}
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
	std::vector<I32> x_coordinates = {100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700};
	std::vector<I32> y_coordinates = {100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700};
	std::vector<String> spells = {"curseball", "curseball", "curseball", "fastball", "fastball", "fastball", "healthpotion"};

	MutAccessUnique<PickupTimer>::access_unique().start();
	generator.initialize_deques(x_coordinates, y_coordinates, spells);
	return true;
}

void PickupSystem::process_signal(PickupTimeoutSignal& signal)
{
	auto& generator = MutAccessUnique<Generator>::access_unique();
	I32 x = generator.next_x();
	I32 y = generator.next_y();
	String spell_name = generator.next_spell();

	if (spell_name == "curseball")
	{
		spawn()
			.with<Pickup>("curseball" , 24.0f, 3)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/CurseballTome"))
			.with<Scale>(geometry::Vec2{0.8f, 0.8f})
			.with<Position>(geometry::Vec2{ x, y })
			.with<Visibility>(true);
	}
	else if (spell_name == "fastball")
	{
		spawn()
			.with<Pickup>("fastball" , 24.0f, 6)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/FastballTome"))
			.with<Scale>(geometry::Vec2{0.8f, 0.8f})
			.with<Position>(geometry::Vec2{ x, y })
			.with<Visibility>(true);
	}
	else if (spell_name == "healthpotion")
	{
		spawn()
			.with<Pickup>("healthpotion", 24.0f, 0)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/HealthPotion"))
			.with<Scale>(geometry::Vec2{1.2f, 1.2f})
			.with<Position>(geometry::Vec2{ x, y })
			.with<Visibility>(true);
	}
}
