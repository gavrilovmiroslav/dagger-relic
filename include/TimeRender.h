#pragma once

#include "Prelude.h"
#include "Common.h"
#include "SDL_ttf.h"
#include "EnemyFSMController.h"

struct TimeRender
{
	TTF_Font* font;
	String text;
	U32 font_size;
	SDL_Color text_color = {0, 0, 0};
	TimeRender(String text, U32 font_size);

	static void init();
	static void deinit();
};

struct TimeRenderControlSystem
	: public ecs::System
	, public MutAccessUnique<WindowingState>
	, public MutAccessGroupStorage<TimeRender, Position, Sprite>

{
	void on_tick() override;
};