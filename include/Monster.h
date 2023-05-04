#pragma once

#include "Definitions.h"
#include "Access.h"
#include "GameComponents.h"

struct MonsterSystem
	: public ecs::System
	, public MutAccessGroupStorage<Monster, KeyBindings, Position, Sprite>
	, public MutAccessGroupStorage<Player, Position>
	, public MutAccessUnique<OurGlobalVar>
{
	using QueryPlayer = MutAccessGroupStorage<Player, Position>;
	using QueryMonster = MutAccessGroupStorage<Monster, KeyBindings, Position, Sprite>;
	void on_tick() override;
};
