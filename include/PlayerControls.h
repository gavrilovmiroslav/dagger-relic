#pragma once

#include "Definitions.h"
#include "Access.h"
#include "GameComponents.h"
#include "PlayerFSM.h"
#include <iostream>
#include "Fight.h"

struct PlayerControlsSystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, KeyBindings, Position, SpriteAnimation>
	, public MutAccessUnique<OurGlobalVar>
	, public MutAccessComponentById<SpriteAnimation>
	, public MutAccessComponentById<Monster>
	, public MutAccessComponentById<Status>
	, public SignalEmitter<HealthUpdateSignal>
	, public MutAccessComponentById<Position>
	, public MutAccessComponentById<KeyBindings>
	, public AccessComponentById<Player>
	, public SignalProcessor<PlayerCollisionSignal>
{
	ecs::Entity player_entity_cache;
	PlayerFSM fsm;
	Bool doing_damage = false;
	Bool fighting = false;
	bool side = true;

	PlayerControlsSystem()
	{
		fsm.events_for_action("TryMove").connect<&PlayerControlsSystem::TryMove>(this);
		fsm.events_for_action("TryStop").connect<&PlayerControlsSystem::TryStop>(this);
		fsm.events_for_action("TryAttack").connect<&PlayerControlsSystem::TryAttack>(this);
		fsm.events_for_action("TryHit").connect<&PlayerControlsSystem::TryHit>(this);
		fsm.current_state = "Idle";
	}

	using HealthUpdateEmitter = SignalEmitter<HealthUpdateSignal>;

	void TryMove()
	{
		auto& sprite = MutAccessComponentById<SpriteAnimation>::get(player_entity_cache);
		if (side)
			sprite.change_to("fallingHero/runningRight");
		else
			sprite.change_to("fallingHero/runningLeft");
		auto& pos = MutAccessComponentById<Position>::get(player_entity_cache);
		auto& bindings = MutAccessComponentById<KeyBindings>::get(player_entity_cache);

		geometry::Vec2 xy { 0.0f, 0.0f };
		F32 speed = 1.0f;

		const auto &keys = KeyState::get();
		if (keys.is_down(bindings.left))
		{
			side = false;
			xy.x -= speed;
			pos.xy.x = std::max(pos.xy.x, float(-PLAYER_OFFSET_LEFT));
		}

		if (keys.is_down(bindings.right))
		{
			side = true;
			xy.x += speed;
			pos.xy.x = std::min(pos.xy.x, float(SCREEN_WIDTH - PLAYER_OFFSET_RIGHT));
		}

		if (glm::length(xy) > 0.0f)
		{
			pos.xy += glm::normalize(xy) * SPEED_MOD * Time::delta_time();
		}
	}

	void TryStop()
	{
		auto& sprite = MutAccessComponentById<SpriteAnimation>::get(player_entity_cache);
		fighting = false;
		doing_damage = false;
		if (side)
			sprite.change_to("fallingHero/heroStanding");
		else
			sprite.change_to("fallingHero/leftHeroStanding");
	}

	void TryAttack()
	{
		auto& sprite = MutAccessComponentById<SpriteAnimation>::get(player_entity_cache);
		fighting = true;
		if (side)
			sprite.change_to("fallingHero/attack");
		if (!side)
			sprite.change_to("fallingHero/attackLeft");
	}

	void TryHit()
	{
		auto& sprite = MutAccessComponentById<SpriteAnimation>::get(player_entity_cache);
		doing_damage = sprite.current_frame >= 4 && sprite.current_frame <= 5;
	}

	void process_signal(PlayerCollisionSignal& signal)
	{
		if (doing_damage && fighting)
		{
			fighting = false;
			doing_damage = false;
			auto& status = MutAccessComponentById<Status>::get(signal.enemy);
			status.health -= 10;
			HealthUpdateEmitter::emit(HealthUpdateSignal{signal.enemy, status.health});
			if (status.health <= 0)
			{
				despawn(signal.enemy);
			}
		}
	}

	void on_tick() override;
};
