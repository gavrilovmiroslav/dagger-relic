#pragma once

#include "Definitions.h"
#include "Access.h"
#include "Components.h"

struct PlatformSystem 
	: public ecs::System
	, public MutAccessGroupStorage<Player, Position>
	, public AccessGroupStorage<Platform, Position>
	, public MutAccessGroupStorage<Box, Position>
{
	using QueryPlayers = MutAccessGroupStorage<Player, Position>;
	using QueryPlatforms = AccessGroupStorage<Platform, Position>;
	using QueryBoxes = MutAccessGroupStorage<Box, Position>;
	
	void on_tick() override;
};