#include "ScoreRender.h"
#include <iostream>

ScoreRender::ScoreRender(String text, U32 font_size)
    : text(text), font_size(font_size)
{
	font = TTF_OpenFont("data/fonts/digital-7.ttf", font_size);
	if (!font)
	{
		spdlog::warn("Error loading font: {}", TTF_GetError());
	}
}

void ScoreRender::init()
{
	if (TTF_Init() < 0)
	{
		spdlog::error("Error intializing SDL_ttf: {}", TTF_GetError());
	}
}

void ScoreRender::deinit()
{
	TTF_Quit();
}

String textToWrite = "";

void ScoreRenderControlSystem::on_tick()
{
	auto &ourGlobal = MutAccessUnique<OurGlobalVar>::access_unique();

	for (auto &&[entity, time_render, position, sprite] : access_storage().each())
	{
		sprite.depth = -10;
		if (!time_render.font)
		{
			continue;
		}
		if (ourGlobal.didPlayerWon)
		{
			int seconds = (ourGlobal.timeMs / 1000) % 60;
			int score = (1000/seconds) * ourGlobal.coinNumber * ourGlobal.playerHealth/10;
			textToWrite = "Your score is: " + std::to_string(score);
		}
		else
		{
			textToWrite = "GAME OVER";
		}
		time_render.text = textToWrite;

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
