#pragma once

#include "Definitions.h"
#include "Access.h"
#include "GameComponents.h"

struct BackgroundSystem
	: public ecs::System
	, public MutAccessGroupStorage<Background, KeyBindings, Position, Sprite>
	, public MutAccessUnique<OurGlobalVar>
{
	void on_tick() override;
};
