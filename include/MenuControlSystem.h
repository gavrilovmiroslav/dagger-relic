#pragma once

#include "MenuFSM.h"
#include "GameComponents.h"

struct MenuClick
{
	MenuFSM m_fsm;
};

struct MenuControlSystem
	: public ecs::System
	, public MutAccessGroupStorage<MenuClick, KeyBinding>
{
	void on_tick() override;
};
