#pragma once

#include "Definitions.h"
#include "Access.h"
#include "GameComponents.h"

struct PhysicsSystem
	: public ecs::System
	, public AccessGroupStorage<Player, Position>
	, public MutAccessGroupStorage<Monster, Position>
	, public SignalEmitter<PlayerCollisionSignal>
	, public MutAccessUnique<OurGlobalVar>
{
	using QueryPlayers = AccessGroupStorage<Player, Position>;
	using QueryEnemies = MutAccessGroupStorage<Monster, Position>;
	using PlayerCollisionEmitter = SignalEmitter<PlayerCollisionSignal>;

	void on_tick() override;
	Bool intersects(const geometry::Vec2& player_center, const geometry::Vec2& enemy_center, float enemy_radius)
	{
		static constexpr const int player_width = 54;
		static constexpr const int player_height = 54;

		const geometry::Vec2 circle_distance{
			abs(enemy_center.x - player_center.x),
			abs(enemy_center.y - player_center.y)
		};

		if (circle_distance.x > (player_width + enemy_radius) || circle_distance.y > (player_height + enemy_radius))
		{
			return false;
		}

		if (circle_distance.x <= player_width || circle_distance.y <= player_height)
		{
			return true;
		}

		const auto dx = circle_distance.x - player_width;
		const auto dy = circle_distance.y - player_height;
		const auto corner_distance = dx * dx + dy * dy;

		return corner_distance <= enemy_radius * enemy_radius;
	}
};

