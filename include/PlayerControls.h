#pragma once

#include "Definitions.h"
#include "Access.h"
#include "GameComponents.h"

struct PlayerControlsSystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, KeyBindings, Position, SpriteAnimation>
	, public MutAccessUnique<OurGlobalVar>
{
	void on_tick() override;
};
