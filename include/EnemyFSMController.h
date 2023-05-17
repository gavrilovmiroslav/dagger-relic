#pragma once

#define ENEMY_SPEED_MOD 100.0f
#define ENEMY_DISTANCER 90.0f

#include "EnemyFSM.h"
// #include "Enemies.h"
#include "Prelude.h"

struct EnemyFSMController : public ecs::System,
							public MutAccessStorage<Player>,
							public MutAccessStorage<Enemy>,
							public MutAccessComponentById<Position>,
							public MutAccessGroupStorage<Enemy, Position, SpriteAnimation>,
							public MutAccessComponentById<SpriteAnimation>,
							public MutAccessComponentById<Flip>,
							public MutAccessComponentById<Enemy>

{

	using QueryEnemies = MutAccessGroupStorage<Enemy, Position, SpriteAnimation>;

	EnemyFSM fsm;
	ecs::Entity enemy_entity_cache;
	Bool doing_damage = false;

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

		Bool should_exit = true;
		for (auto &&[entity2, enemy2, pos2, sprite] : QueryEnemies::access_storage().each())
		{
			if (entity2 == enemy_entity_cache)
			{
				should_exit = false;
				break;
			}
		}

		if (should_exit)
		{
			return;
		}

		Flip &flip = MutAccessComponentById<Flip>::get(enemy_entity_cache);
		ecs::Entity player = MutAccessStorage<Player>::access_storage().front();
		Position &player_position = MutAccessComponentById<Position>::get(player);
		Enemy &enemy = MutAccessComponentById<Enemy>::get(enemy_entity_cache);
		Position &pos = MutAccessComponentById<Position>::get(enemy_entity_cache);

		auto &sprite = MutAccessComponentById<SpriteAnimation>::get(enemy_entity_cache);
		sprite.change_to("Golem/Golem1_run");

		auto size = QueryEnemies::access_storage().size_hint();
		// enemy flocking system (do this only if there is more then one enemy left)
		if (size >= 2)
		{

			auto min_distance = INFINITY;
			Position *nearest_enemy_pos = nullptr;

			for (auto &&[entity2, enemy2, pos2, sprite] : QueryEnemies::access_storage().each())
			{
				if (enemy_entity_cache != entity2)
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
			if (should_flip())
			{
				flip = Flip::Horizontal;
			}
			else
			{
				flip = Flip::None;
			}
			enemy.speed = normalize(enemy.speed);
			pos.xy += enemy.speed * Time::delta_time() * enemy.movement_speed;
		}
		else
		{

			auto to_player = normalize(player_position.xy - pos.xy);
			enemy.speed += to_player;
			if (should_flip())
			{
				flip = Flip::Horizontal;
			}
			else
			{
				flip = Flip::None;
			}
			enemy.speed = normalize(enemy.speed);
			pos.xy += enemy.speed * Time::delta_time() * enemy.movement_speed;
		}
		if (enemy.speed.length == 0)
		{
			Logger::critical("Speed is 0 len");
		}
		else if (pos.xy.length == 0)
		{
			Logger::critical("Position is 0 len");
		}
		// Logger::info("enemy {}. position {},{} , speed {}, {}", (int)enemy_entity_cache, pos.xy.x, pos.xy.y, enemy.speed.x, enemy.speed.y);
	}

	void TryStop()
	{
		Bool should_exit = true;
		for (auto &&[entity2, enemy2, pos2, sprite] : QueryEnemies::access_storage().each())
		{
			if (entity2 == enemy_entity_cache)
			{
				should_exit = false;
				break;
			}
		}

		if (should_exit)
		{
			return;
		}
		
		auto &sprite = MutAccessComponentById<SpriteAnimation>::get(enemy_entity_cache);
		sprite.change_to("Golem/Golem1_idle");
		Flip &flip = MutAccessComponentById<Flip>::get(enemy_entity_cache);

		if (should_flip())
		{
			flip = Flip::Horizontal;
		}
		else
		{
			flip = Flip::None;
		}
	}

	void TryAttack()
	{
		Bool should_exit = true;
		for (auto &&[entity2, enemy2, pos2, sprite] : QueryEnemies::access_storage().each())
		{
			if (entity2 == enemy_entity_cache)
			{
				should_exit = false;
				break;
			}
		}

		if (should_exit)
		{
			return;
		}
		
		auto &sprite = MutAccessComponentById<SpriteAnimation>::get(enemy_entity_cache);
		sprite.change_to("Golem/Golem1_attack");
		Flip &flip = MutAccessComponentById<Flip>::get(enemy_entity_cache);
		ecs::Entity player = MutAccessStorage<Player>::access_storage().front();
		Position &player_position = MutAccessComponentById<Position>::get(player);
		Position &pos = MutAccessComponentById<Position>::get(enemy_entity_cache);

		if (should_flip())
		{
			flip = Flip::Horizontal;
		}
		else
		{
			flip = Flip::None;
		}
		doing_damage = true;
	}

	void TryHit()
	{
		
		Bool should_exit = true;
		for (auto &&[entity2, enemy2, pos2, sprite] : QueryEnemies::access_storage().each())
		{
			if (entity2 == enemy_entity_cache)
			{
				should_exit = false;
				break;
			}
		}

		if (should_exit)
		{
			return;
		}

		auto &sprite = MutAccessComponentById<SpriteAnimation>::get(enemy_entity_cache);
		sprite.change_to("Golem/Golem1_attack");

		ecs::Entity player = MutAccessStorage<Player>::access_storage().front();
		Position &player_position = MutAccessComponentById<Position>::get(player);
		Position &pos = MutAccessComponentById<Position>::get(enemy_entity_cache);

		if (distance(player_position.xy, pos.xy) < 50.0f && sprite.current_frame == 8 && doing_damage)
		{
			spawn()
				.with<Damage>(enemy_entity_cache, player, 10)
				.done();
		}

		doing_damage = false;
	}

	inline Bool should_flip() const
	{
		Position &pos = MutAccessComponentById<Position>::get(enemy_entity_cache);
		ecs::Entity player = MutAccessStorage<Player>::access_storage().front();
		Position &player_position = MutAccessComponentById<Position>::get(player);
		return player_position.xy.x - pos.xy.x < 0;
	}

	geometry::Vec2 my_normalize(geometry::Vec2 vec)
	{
		if (vec.length == 0)
		{
			return geometry::Vec2(0.0f, 0.0f);
		}

		float magnitude = std::sqrt(vec.x * vec.x + vec.y * vec.y);
		if (magnitude != 0)
		{
			vec.x /= magnitude;
			vec.y /= magnitude;
		}
		return vec;
	}
};
