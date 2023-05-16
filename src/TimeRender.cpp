#include "TimeRender.h"
#include <iostream>

TimeRender::TimeRender(String text, U32 font_size)
    : text(text), font_size(font_size)
{
	font = TTF_OpenFont("data/fonts/digital-7.ttf", font_size);
	if (!font)
	{
		spdlog::warn("Error loading font: {}", TTF_GetError());
	}
}

void TimeRender::init()
{
	if (TTF_Init() < 0)
	{
		spdlog::error("Error intializing SDL_ttf: {}", TTF_GetError());
	}
}

void TimeRender::deinit()
{
	TTF_Quit();
}

auto start = std::chrono::high_resolution_clock::now(); // get current time
int elapsed_time_ms = 0;

void TimeRenderControlSystem::on_tick()
{
	for (auto &&[entity, time_render, position, sprite] : access_storage().each())
	{
		sprite.depth = -10;
		if (!time_render.font)
		{
			continue;
		}

		auto end = std::chrono::high_resolution_clock::now(); // get current time
       		elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		int minutes = elapsed_time_ms / (1000 * 60);
       	 	int seconds = (elapsed_time_ms / 1000) % 60;
        	int milliseconds = elapsed_time_ms % 1000;
		String time = "" + std::to_string(minutes) + ":" +  std::to_string(seconds) + ":" +  std::to_string(milliseconds);
		time_render.text = time;
		int oldTime = elapsed_time_ms;

		auto &state = MutAccessUnique<WindowingState>::access_unique();
		SDL_Surface *text_surf = TTF_RenderText_Solid(time_render.font, time_render.text.c_str(), time_render.text_color);
		SDL_Texture *texture = SDL_CreateTextureFromSurface(state.renderer, text_surf);
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
