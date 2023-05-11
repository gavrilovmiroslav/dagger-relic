#pragma once

#include "Access.h"
#include "Prelude.h"
#include "GameComponents.h"
#include "MenuFSM.h"

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
