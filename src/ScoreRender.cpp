// #include "Common.h"
#include "ScoreRender.h"
#include <iostream>

ScoreRender::ScoreRender(String text, U32 font_size)
    : text(text), font_size(font_size)
{
    font = TTF_OpenFont("./data/fonts/Minecraft.ttf", font_size);

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

void ScoreRenderControlSystem::on_tick()
{

    auto player = MutAccessStorage<Player>::access_storage().front();
    auto score = MutAccessComponentById<Score>::get(player).score;

    for (auto &&[entity, Score_render, position, sprite] : MutAccessGroupStorage<ScoreRender, Position, Sprite>::access_storage().each())
    {
        sprite.depth = -7;
        if (!Score_render.font)
        {
            continue;
        }

        Score_render.text = "Score: " + std::to_string(score);

        auto &state = MutAccessUnique<WindowingState>::access_unique();
        SDL_Surface *text_surf = TTF_RenderText_Solid(Score_render.font, Score_render.text.c_str(), Score_render.text_color);
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