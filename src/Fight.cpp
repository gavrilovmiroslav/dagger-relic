#include "Fight.h"

void PhysicsSystem::on_tick()
{
	for (auto&& [enemy_entity, enemy, pos] : QueryEnemies::access_storage().each())
	{
		for (auto&& [player_entity, player, player_pos] : QueryPlayers::access_storage().each())
		{
			if (intersects(geometry::Vec2{player_pos.xy.x,player_pos.xy.y},
				       geometry::Vec2{pos.xy.x + 120,pos.xy.y + 120}, 60))
			{
				PlayerCollisionEmitter::emit(PlayerCollisionSignal{ enemy_entity, player_entity});
			}
		}
	}
}
