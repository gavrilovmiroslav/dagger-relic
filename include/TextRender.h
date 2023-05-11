#pragma once

#include "Prelude.h"
#include "SDL_ttf.h"

struct TextRender
{
	TTF_Font* font;
	String text;
	U32 font_size;
	SDL_Color text_color = {102, 102, 0};
	TextRender(String text, U32 font_size);

	static void init();
	static void deinit();
};

struct TextRenderControlSystem
	: public ecs::System,
	  public MutAccessUnique<WindowingState>,
	  public MutAccessGroupStorage<TextRender, Position>
{
	void on_tick() override;
};
