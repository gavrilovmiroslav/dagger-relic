#pragma once
#include "Prelude.h"

struct EnemyMovementSystem
	: public ecs::System
	, public MutAccessGroupStorage<Enemy, Position,SpriteAnimation>
 	, public AccessGroupStorage<Player, Position>
	, public MutAccessComponentById<Enemy>
	, public MutAccessComponentById<Position>
	, public SignalProcessor<PlayerCollisionSignal>
{
	using QueryPlayers = AccessGroupStorage<Player, Position>;
	using QueryEnemies = MutAccessGroupStorage<Enemy, Position,SpriteAnimation>;
	
	virtual void process_signal(PlayerCollisionSignal& signal)
	{
		auto& enemy = MutAccessComponentById<Enemy>::get(signal.enemy);
		auto& enemy_pos = MutAccessComponentById<Position>::get(signal.enemy);
		
		enemy.speed *= -1;
		enemy_pos.xy += enemy.speed * ENEMY_SPEED_MOD;
	}

	void on_tick() override
	{

		auto player  = QueryPlayers::access_storage().front();
		auto player_position = MutAccessComponentById<Position>::get(player);
		
		//enemy flocking system
		for (auto&& [entity, enemy, pos,sprite] : QueryEnemies::access_storage().each())
		{
			auto min_distance = INFINITY;
			Position* nearest_enemy_pos = nullptr;

			for (auto&& [entity2, enemy2, pos2,sprite] : QueryEnemies::access_storage().each())
			{
				if(entity != entity2)
				{
					auto dist = distance(pos.xy, pos2.xy);
					if (dist < min_distance)
					{
						min_distance = dist;
						nearest_enemy_pos = &pos2;
					}
				}
			}
			
			auto to_player =  normalize(player_position.xy - pos.xy);
			auto from_nearest_enemy  = normalize(pos.xy - nearest_enemy_pos->xy);

			// auto dist_pl_en = distance(pos.xy,player_position.xy);
			// if(dist_pl_en < 80.0f){
			// 	sprite.change_to("Skeleton/Skeleton_attack");
			// }

			auto enemy_tilt_coef = 0.02f;
			std::cout << min_distance << std::endl;
			if (min_distance < 50.0f)
			{
				enemy_tilt_coef = 1.0f;
				std::cout << "to close" << std::endl;
			}

			enemy.speed += to_player + from_nearest_enemy * enemy_tilt_coef;
			enemy.speed = normalize(enemy.speed);

			pos.xy += enemy.speed * Time::delta_time() * ENEMY_SPEED_MOD;
		}
	}
};
