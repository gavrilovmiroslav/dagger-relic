#pragma once

#include "Definitions.h"
#include "Access.h"
#include "GameSpecificComponents.h"

struct PlayerControlsSystem 
	: public ecs::System
	, public MutAccessGroupStorage<Player, KeyBindings, Position, Flip>
{
	void on_tick() override;
};