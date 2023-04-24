#pragma once

#include "Definitions.h"
#include "Access.h"
#include "GameSpecificComponents.h"

struct GravitySystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, Position>
{
	void on_tick() override;
};
