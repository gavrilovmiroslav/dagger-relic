#pragma once

#include "Definitions.h"
#include "Access.h"
#include "GameComponents.h"

struct ItemSystem
    : public ecs::System,
      public MutAccessGroupStorage<Item, KeyBindings, Position, Sprite, Visibility>,
      public MutAccessGroupStorage<Player, Position>,
      public MutAccessUnique<OurGlobalVar>
{
	using QueryPlayer = MutAccessGroupStorage<Player, Position>;
	using QueryItem = MutAccessGroupStorage<Item, KeyBindings, Position, Sprite, Visibility>;
	void on_tick() override;
};
