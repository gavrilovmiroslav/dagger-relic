#pragma once

#include "Definitions.h"
#include "Access.h"
#include "GameSpecificComponents.h"

struct PickupSystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, Position>
	, public MutAccessGroupStorage<Pickup, Position>
    , public MutAccessComponentById<Item>
    , public MutAccessComponentById<Visibility>
{
	using QueryPlayers = MutAccessGroupStorage<Player, Position>;
	using QueryPickups = MutAccessGroupStorage<Pickup, Position>;
    using ItemGetter = MutAccessComponentById<Item>;
	
	void on_tick() override;
};
