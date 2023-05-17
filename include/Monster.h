#pragma once

#include "Definitions.h"
#include "Access.h"
#include "GameComponents.h"
#include "MonsterFSM.h"

struct MonsterSystem
	: public ecs::System
	, public MutAccessGroupStorage<Monster, KeyBindings, Position, Sprite, SpriteAnimation>
	, public MutAccessGroupStorage<Player, Position>
	, public MutAccessUnique<OurGlobalVar>
	, public MutAccessComponentById<SpriteAnimation>
	, public MutAccessComponentById<Player>
	, public MutAccessComponentById<Status>
	, public SignalEmitter<HealthUpdateSignal>
	, public MutAccessComponentById<Position>
	, public MutAccessComponentById<Flip>
	, public AccessComponentById<Monster>
	, public SignalProcessor<PlayerCollisionSignal>
{
	using QueryPlayer = MutAccessGroupStorage<Player, Position>;
	using QueryMonster = MutAccessGroupStorage<Monster, KeyBindings, Position, Sprite, SpriteAnimation>;

	ecs::Entity monster_entity_cache;
	ecs::Entity player_entity_cache;
	MonsterFSM fsm;
	Bool doing_damage = false;
	Bool fighting = false;
	Bool side = true;
	Bool goLeft = true;

	MonsterSystem()
	{
		fsm.events_for_action("TryMove").connect<&MonsterSystem::TryMove>(this);
		fsm.events_for_action("TryStop").connect<&MonsterSystem::TryStop>(this);
		fsm.events_for_action("TryAttack").connect<&MonsterSystem::TryAttack>(this);
		fsm.events_for_action("TryHit").connect<&MonsterSystem::TryHit>(this);
		fsm.current_state = "Idle";
	}
	using HealthUpdateEmitter = SignalEmitter<HealthUpdateSignal>;

	void TryMove()
	{
		auto& sprite = MutAccessComponentById<SpriteAnimation>::get(monster_entity_cache);
		sprite.change_to("monster/idle");

		auto& flip = MutAccessComponentById<Flip>::get(monster_entity_cache);
		auto& pos = MutAccessComponentById<Position>::get(monster_entity_cache);

		geometry::Vec2 xy { 0.0f, 0.0f };
		F32 speed = 0.1f;

		if (goLeft)
		{
			flip = Flip::Horizontal;
			side = false;
			xy.x -= speed;
			if (pos.xy.x < -MONSTER_OFFSET_LEFT)
				goLeft = false;
		}
		else if (!goLeft)
		{
			flip = Flip::None;
			side = true;
			xy.x += speed;
			if (pos.xy.x > SCREEN_WIDTH - MONSTER_OFFSET_RIGHT)
				goLeft = true;
		}
		fighting = false;
		doing_damage = false;
		if (glm::length(xy) > 0.0f)
		{
			pos.xy += glm::normalize(xy) * (SPEED_MOD-200) * Time::delta_time();
		}
	}

	void TryStop()
	{
		fighting = false;
		doing_damage = false;
	}

	void TryAttack()
	{
		auto& sprite = MutAccessComponentById<SpriteAnimation>::get(monster_entity_cache);
		auto& flip = MutAccessComponentById<Flip>::get(monster_entity_cache);
		fighting = true;
		sprite.change_to("monster/attacking");
	}

	void TryHit()
	{
		auto& sprite = MutAccessComponentById<SpriteAnimation>::get(monster_entity_cache);
		doing_damage = sprite.current_frame >= 3 && sprite.current_frame <= 5;
	}

	void process_signal(PlayerCollisionSignal& signal)
	{
		if (doing_damage && fighting)
		{
			fighting = false;
			doing_damage = false;
			auto& status = MutAccessComponentById<Status>::get(signal.player);
			status.health -= 10;
			HealthUpdateEmitter::emit(HealthUpdateSignal{signal.player, status.health});
			if (status.health <= 0)
			{
				despawn(signal.player);
			}
		}
	}

	void on_tick() override;
};
