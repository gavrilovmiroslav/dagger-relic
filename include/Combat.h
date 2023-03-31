#pragma once

#include "Definitions.h"
#include "Access.h"
#include "GameSpecificComponents.h"

struct CombatSystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, Position>
	, public MutAccessGroupStorage<BasicAttack, Position, SpriteAnimation, Flip>
{
	using QueryPlayers = MutAccessGroupStorage<Player, Position>;
	using QueryAttacks = MutAccessGroupStorage<BasicAttack, Position, SpriteAnimation, Flip>;

	void on_tick() override;
};
