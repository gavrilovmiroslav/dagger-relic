#pragma once

#define ENEMY_SPEED_MOD 50.0f
#define ENEMY_DISTANCER 90.0f

#include "EnemyFSM.h"
#include "Enemies.h"
#include "Prelude.h"

struct EnemyFSMController : public ecs::System, 
                            public MutAccessStorage<Player>,
                            public MutAccessComponentById<Position>,
                            public MutAccessGroupStorage<Enemy, Position,SpriteAnimation>,
                            public MutAccessComponentById<EnemyFSMController>,
                            public MutAccessStorage<EnemyFSMController>,
                            public MutAccessComponentById<SpriteAnimation>,
							public MutAccessComponentById<Flip> 

{

    using QueryEnemies = MutAccessGroupStorage<Enemy, Position,SpriteAnimation>;
    
    EnemyFSM fsm;
    ecs::Entity enemy_entity_cache;

    EnemyFSMController(ecs::Entity entt)
        : enemy_entity_cache(entt)
    {
        fsm.events_for_action("TryMove").connect<&EnemyFSMController::TryMove>(this);
        fsm.events_for_action("TryStop").connect<&EnemyFSMController::TryStop>(this);
        fsm.events_for_action("TryAttack").connect<&EnemyFSMController::TryAttack>(this);	
        fsm.events_for_action("TryHit").connect<&EnemyFSMController::TryHit>(this);
        fsm.current_state = "Idle";
    }


    	void TryMove() 
	{

		auto &sprite = MutAccessComponentById<SpriteAnimation>::get(enemy_entity_cache);
		sprite.change_to("Golem/Golem1_run");
		
		auto player = MutAccessStorage<Player>::access_storage().front();
		auto &player_position = MutAccessComponentById<Position>::get(player);
		auto size = QueryEnemies::access_storage().size_hint();
		auto &flip = MutAccessComponentById<Flip>::get(enemy_entity_cache);


		//enemy flocking system (do this only if there is more then one enemy left)
		if(size >= 2)
		{
			for (auto&& [entity, enemy, pos,sprite] : QueryEnemies::access_storage().each())
			{	
				auto min_distance = INFINITY;
				Position* nearest_enemy_pos = nullptr;

				for (auto&& [entity2, enemy2, pos2,sprite] : QueryEnemies::access_storage().each())
				{
					if(entity != entity2)
					{
						auto dist = glm::distance(pos.xy, pos2.xy);
						if (dist < min_distance)
						{
							min_distance = dist;
							nearest_enemy_pos = &pos2;
						}
					}
				}
				
				// TODO: check length around this...
				auto to_player = normalize(player_position.xy - pos.xy);
				auto from_nearest_enemy = normalize(pos.xy - nearest_enemy_pos->xy);


				auto enemy_tilt_coef = 0.02f;

				if (min_distance < ENEMY_DISTANCER)
				{
					enemy_tilt_coef = 1.0f;
				}
				

				enemy.speed += to_player + from_nearest_enemy * enemy_tilt_coef;
				if(enemy.speed.x <= 0)
				{
					flip = Flip::Horizontal;

				}
				else
				{
					flip = Flip::None;
				}
				enemy.speed = normalize(enemy.speed);
				pos.xy += enemy.speed * Time::delta_time() * ENEMY_SPEED_MOD;

			}
		}
		else 
		{
			for (auto&& [entity, enemy, pos,sprite] : QueryEnemies::access_storage().each())
			{
				auto to_player =  normalize(player_position.xy - pos.xy);
				enemy.speed += to_player;
				enemy.speed = normalize(enemy.speed);
				pos.xy += enemy.speed * Time::delta_time() * ENEMY_SPEED_MOD;
			}

		}
	}

	void TryStop() 
	{
		auto &sprite = MutAccessComponentById<SpriteAnimation>::get(enemy_entity_cache);
		sprite.change_to("Golem/Golem1_idle");
	}

	void TryAttack()
	{
		auto &sprite = MutAccessComponentById<SpriteAnimation>::get(enemy_entity_cache);
		sprite.change_to("Golem/Golem1_attack");
	}

	void TryHit()
	{

	}

};


