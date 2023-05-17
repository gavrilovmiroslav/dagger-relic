#pragma once

#include "Prelude.h"
#include "SDL_ttf.h"
#include "Definitions.h"
#include "Access.h"
#include "GameComponents.h"



struct ScoreRenderControlSystem
	: public ecs::System
	, public MutAccessUnique<WindowingState>
	, public MutAccessGroupStorage<ScoreRender, Position, Sprite>
	, public MutAccessUnique<OurGlobalVar>
{
	void on_tick() override;
};
