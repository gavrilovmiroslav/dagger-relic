#include "TextRender.h"

TextRender::TextRender(String text, U32 font_size)
			: text(text)
			, font_size(font_size)
{
	font = TTF_OpenFont("data/fonts/Roboto-Black.ttf", font_size);
	if (!font)
	{
		spdlog::warn("Error loading font: {}", TTF_GetError());
	}
}

void TextRender::init()
{
	if (TTF_Init() < 0)
	{
		spdlog::error("Error intializing SDL_ttf: {}", TTF_GetError());
	}
}

void TextRender::deinit()
{
	TTF_Quit();
}

void TextRenderControlSystem::on_tick()
{
	for (auto&& [entity, text_render, position] : access_storage().each())
	{
		if (!text_render.font)
		{
			continue;
		}
		auto& state = MutAccessUnique<WindowingState>::access_unique();
		SDL_Surface* text_surf = TTF_RenderText_Solid(text_render.font, text_render.text.c_str(), text_render.text_color);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(state.renderer, text_surf);
		SDL_Rect dest;
		dest.x = position.xy.x;
		dest.y = position.xy.y;
		dest.w = text_surf->w;
		dest.h = text_surf->h;
		SDL_RenderCopy(state.renderer, texture, nullptr, &dest);
		SDL_DestroyTexture(texture);
		SDL_FreeSurface(text_surf);
		SDL_RenderPresent(state.renderer);
	}
}
