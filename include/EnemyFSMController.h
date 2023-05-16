#pragma once

#define ENEMY_SPEED_MOD 100.0f
#define ENEMY_DISTANCER 90.0f

#include "EnemyFSM.h"
#include "Prelude.h"

struct EnemyFSMInstance
{
	String current_state;
	String queued_command;

	EnemyFSMInstance(String&& current)
		: current_state{current}
		, queued_command{""}
	{}
};

struct EnemyFSMController 	
	: public ecs::System
	, public AllocateUnique<EnemyFSM>
	, public MutAccessUnique<EnemyFSM>
	, public AccessStorage<Player>
	, public MutAccessComponentById<Position>
	, public MutAccessGroupStorage<Enemy, Position, SpriteAnimation>
	, public MutAccessComponentById<SpriteAnimation>
	, public MutAccessComponentById<Flip>
	, public MutAccessComponentById<Enemy>
	, public MutAccessStorage<EnemyFSMInstance>
{
	using QueryEnemies = MutAccessGroupStorage<Enemy, Position, SpriteAnimation>;

	ecs::Entity enemy_entity_cache;
	Bool doing_damage = false;

	EnemyFSMController()
		: enemy_entity_cache(ecs::no_entity)
	{
		auto& fsm = MutAccessUnique<EnemyFSM>::access_unique();
		fsm.events_for_action("TryMove").connect<&EnemyFSMController::try_move>(this);
		fsm.events_for_action("TryStop").connect<&EnemyFSMController::try_stop>(this);
		fsm.events_for_action("TryAttack").connect<&EnemyFSMController::try_attack>(this);
		fsm.events_for_action("TryHit").connect<&EnemyFSMController::try_hit>(this);
		fsm.current_state = "Idle";
	}

	void on_tick() override {
		auto& fsm = MutAccessUnique<EnemyFSM>::access_unique();
		for (auto&& [entity, instance] : MutAccessStorage<EnemyFSMInstance>::access_storage().each())
		{
			enemy_entity_cache = entity;
			if (instance.queued_command != "")
			{
				String next_state;
				fsm.trigger(instance.queued_command, instance.current_state, next_state);
				instance.current_state = next_state;
				instance.queued_command = "";
			}
			enemy_entity_cache = ecs::no_entity;
		}
	}

	ecs::Entity get_player() const
	{
		static ecs::Entity cached_entity = ecs::no_entity;
		if (cached_entity != ecs::no_entity)
		{
			return cached_entity;
		}

		cached_entity = AccessStorage<Player>::access_storage().front();
		return cached_entity;
	}

	void try_move()
	{
		Flip& flip = MutAccessComponentById<Flip>::get(enemy_entity_cache);
		ecs::Entity player = get_player();
		Position& player_position = MutAccessComponentById<Position>::get(player);
		Enemy& enemy = MutAccessComponentById<Enemy>::get(enemy_entity_cache);
		Position& pos = MutAccessComponentById<Position>::get(enemy_entity_cache);

		auto &sprite = MutAccessComponentById<SpriteAnimation>::get(enemy_entity_cache);
		sprite.change_to("Golem/Golem1_run");

		auto size = QueryEnemies::access_storage().size_hint();
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

			auto to_player = my_normalize(player_position.xy - pos.xy);
			auto from_nearest_enemy = my_normalize(pos.xy - nearest_enemy_pos->xy);

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
			enemy.speed = my_normalize(enemy.speed);
			pos.xy += enemy.speed * Time::delta_time() * ENEMY_SPEED_MOD;
		}
		else
		{
			Flip &flip = MutAccessComponentById<Flip>::get(enemy_entity_cache);
			ecs::Entity player = get_player();
			Position &player_position = MutAccessComponentById<Position>::get(player);

			auto to_player = my_normalize(player_position.xy - pos.xy);
			enemy.speed += to_player;
			if (should_flip())
			{
				flip = Flip::Horizontal;
			}
			else
			{
				flip = Flip::None;
			}
			enemy.speed = my_normalize(enemy.speed);
			pos.xy += enemy.speed * Time::delta_time() * ENEMY_SPEED_MOD;
		}

		if (enemy.speed.length == 0)
		{
			Logger::critical("Speed is 0 len");
		}		
		else if (pos.xy.length == 0)
		{
			Logger::critical("Position is 0 len");
		}		
	}

	void try_stop()
	{
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

	void try_attack()
	{
		auto &sprite = MutAccessComponentById<SpriteAnimation>::get(enemy_entity_cache);
		sprite.change_to("Golem/Golem1_attack");
		Flip &flip = MutAccessComponentById<Flip>::get(enemy_entity_cache);
		ecs::Entity player = get_player();
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

	void try_hit()
	{
		auto &sprite = MutAccessComponentById<SpriteAnimation>::get(enemy_entity_cache);
		sprite.change_to("Golem/Golem1_attack");

		ecs::Entity player = get_player();
		Position &player_position = MutAccessComponentById<Position>::get(player);
		Position &pos = MutAccessComponentById<Position>::get(enemy_entity_cache);

		if (distance(player_position.xy, pos.xy) < 50.0f && sprite.current_frame == 8 && doing_damage)
		{
			spawn()
				.with<Damage>(enemy_entity_cache, player,10)
				.done();
		}

		doing_damage = false; 
	}

	inline Bool should_flip() const
	{
		Position &pos = MutAccessComponentById<Position>::get(enemy_entity_cache);
		ecs::Entity player = get_player();
		Position &player_position = MutAccessComponentById<Position>::get(player);
		return (player_position.xy.x - pos.xy.x) < 0.0f;
	}

	geometry::Vec2 my_normalize(geometry::Vec2 vec)
	{
		if (vec.length() == 0.0f)
		{
			return geometry::Vec2(0.0f, 0.0f);
		}

		float magnitude = std::sqrt(vec.x * vec.x + vec.y * vec.y);
		if (magnitude != 0.0f)
		{
			vec.x /= magnitude;
			vec.y /= magnitude;
		}
		return vec;
	}
};
