#pragma once

#include "Definitions.h"
#include "Access.h"
#include "GameComponents.h"

struct PlatformSystem
    : public ecs::System,
      public MutAccessGroupStorage<Platform, KeyBindings, Position, Sprite>,
      public MutAccessGroupStorage<Player, Position>,
      public MutAccessUnique<OurGlobalVar>
{
	using QueryPlayer = MutAccessGroupStorage<Player, Position>;
	using QueryPlatform = MutAccessGroupStorage<Platform, KeyBindings, Position, Sprite>;
	void on_tick() override;
};
