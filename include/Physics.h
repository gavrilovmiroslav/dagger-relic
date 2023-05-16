#pragma once
#include "Prelude.h"

struct PlayerCollisionSignal 
{
	ecs::Entity enemy;
	ecs::Entity player;
};

struct EnemyCollisionSignal
{
	ecs::Entity player;
	ecs::Entity enemy;
};

struct PhysicsSystem
	: public ecs::System,
	  public AccessGroupStorage<Player, Position>
	, public MutAccessGroupStorage<Enemy, Position>
	, public SignalEmitter<PlayerCollisionSignal>
	, public SignalEmitter<EnemyCollisionSignal>

{
	using QueryPlayers = AccessGroupStorage<Player, Position>;
	using QueryEnemies = MutAccessGroupStorage<Enemy, Position>;

	Bool intersects(const geometry::Vec2& player_center, const geometry::Vec2& enemy_center, float enemy_radius)
	{
		static constexpr const int player_width = 11;
		static constexpr const int player_height = 32;

		const geometry::Vec2 circle_distance{
			abs(enemy_center.x - player_center.x),
			abs(enemy_center.y - player_center.y)
		};

		if (circle_distance.x > (player_width + enemy_radius)) { return false; }
		if (circle_distance.y > (player_height + enemy_radius)) { return false; }

		if (circle_distance.x <= player_width) { return true; }
		if (circle_distance.y <= player_height) { return true; }

		const auto dx = circle_distance.x - player_width;
		const auto dy = circle_distance.y - player_height;
		const auto corner_distance = dx * dx + dy * dy;

		return corner_distance <= enemy_radius * enemy_radius;
	}


	void on_tick() override
	{
		for (auto&& [enemy_entity, enemy, pos] : QueryEnemies::access_storage().each())
		{
			for (auto&& [player_entity, player_pos] : QueryPlayers::access_storage().each())
			{
				if (intersects(player_pos.xy, pos.xy, 50))
				{
					SignalEmitter<PlayerCollisionSignal>::emit(PlayerCollisionSignal{ enemy_entity, player_entity});
					SignalEmitter<EnemyCollisionSignal>::emit(EnemyCollisionSignal { player_entity, enemy_entity});
					
				}
			}
		}
	}
};
