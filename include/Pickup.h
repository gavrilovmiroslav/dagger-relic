#pragma once

#include "Definitions.h"
#include "Access.h"
#include "GameSpecificComponents.h"

struct PickupSystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, Position>
	, public MutAccessGroupStorage<Pickup, Position>
{
	using QueryPlayers 	= MutAccessGroupStorage<Player, Position>;
	using QueryPlickups = MutAccessGroupStorage<Pickup, Position>;
	
	void on_tick() override;
};