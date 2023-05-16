#pragma once

#define ATTACKING_RANGE 50.0f

#include "Prelude.h"
#include "EnemyFSM.h"
#include "EnemyFSMController.h"

struct EnemiesController : public ecs::System,
						   public MutAccessStorage<Enemy>,
						   public MutAccessGroupStorage<Enemy, Position, SpriteAnimation, EnemyFSMInstance>,
						   public AccessGroupStorage<Player, Position>,
						   public MutAccessStorage<Player>,
						   public MutAccessComponentById<SpriteAnimation>,
						   public MutAccessComponentById<Position>
{
	ecs::Entity player_entity_cache;
	ecs::Entity enemy_entity_cache;

	using QueryEnemies = MutAccessGroupStorage<Enemy, Position, SpriteAnimation, EnemyFSMInstance>;
	using QueryPlayers = AccessGroupStorage<Player, Position>;

	void on_tick() override 
	{
		for (auto&& [enemy_entity, enemy, enemy_position, sprite_animation, fsm_instance] : QueryEnemies::access_storage().each())
		{
			for (auto &&[player_entity, player_position] : QueryPlayers::access_storage().each())
			{
				if(sprite_animation.is_finished())
				{
					fsm_instance.queued_command = "animDone";
				}
				else 
				{
					if(sprite_animation.current_frame == 8)
					{
						fsm_instance.queued_command = "checkForHit";
					}
				}
				
				auto dist = distance(enemy_position.xy, player_position.xy);
				if(dist <= ATTACKING_RANGE)
				{
					fsm_instance.queued_command = "attack";
				}
				else
				{
					fsm_instance.queued_command = "walk";
				}
			}
		}
	}
};
