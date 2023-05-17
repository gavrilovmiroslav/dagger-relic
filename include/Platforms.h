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
	bool is_spawned1 = false;
	bool box_on_flor= false;
	bool knight_on_platform=false;
	bool rougue_on_platform=false;
	
	using QueryPlayers = MutAccessGroupStorage<Player, Position>;
	using QueryPlatforms = AccessGroupStorage<Platform, Position>;
	using QueryBoxes = MutAccessGroupStorage<Box, Position>;
	
	void on_tick() override;
};