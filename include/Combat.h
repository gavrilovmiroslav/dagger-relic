#pragma once

#include "Definitions.h"
#include "Access.h"
#include "GameSpecificComponents.h"

struct ProjectileSpawnSystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, KeyBindings, Position, SpriteAnimation, Flip, Item>
{
	void on_tick() override;
};

struct SpellMovementSystem
	: public ecs::System
	, public MutAccessGroupStorage<Spell, Position, SpriteAnimation, Flip, Move, Clipping>
	, public MutAccessGroupStorage<Platform, Position>
	, public MutAccessGroupStorage<Player, Position>
	, public SignalEmitter<SpellPlatformCollisionSignal>
	, public SignalEmitter<SpellPlayerCollisionSignal>

{
	using QuerySpells = MutAccessGroupStorage<Spell, Position, SpriteAnimation, Flip, Move, Clipping>;
	using QueryPlatforms = MutAccessGroupStorage<Platform, Position>;
	using QueryPlayers = MutAccessGroupStorage<Player, Position>;

	using PlatformCollisionEmitter = SignalEmitter<SpellPlatformCollisionSignal>;
	using PlayerCollisionEmitter = SignalEmitter<SpellPlayerCollisionSignal>;


	void on_tick() override;
};

struct SpellCollisionSystem
	: public ecs::System
	, public SignalProcessor<SpellPlayerCollisionSignal>
	, public AccessEntityValidity
	, public MutAccessComponentById<Duration>
	, public MutAccessComponentById<Status>
	, public MutAccessComponentById<Spell>
	, public SignalEmitter<HealthUpdateSignal>
{
	using HealthUpdateEmitter = SignalEmitter<HealthUpdateSignal>;

	virtual void process_signal(SpellPlayerCollisionSignal& signal);
};
