#pragma once

#include "Prelude.h"
#include "Common.h"
#include "SDL_ttf.h"

struct ScoreRender
{
	TTF_Font* font;
	String text;
	U32 font_size;
	SDL_Color text_color = {0, 0, 0};
	ScoreRender(String text, U32 font_size);

	static void init();
	static void deinit();
};

struct ScoreRenderControlSystem
	: public ecs::System
	, public MutAccessUnique<WindowingState>
	, public MutAccessGroupStorage<ScoreRender, Position, Sprite>
    , public MutAccessStorage<Player>
    , public MutAccessComponentById<Score>

{
	void on_tick() override;
};