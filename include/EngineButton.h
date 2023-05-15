#include <iostream>
#include "SDL_ttf.h"
#include "MainMenu.h"
#include "SDL.h"
#include "Prelude.h"
#include "EventBus.h"

class EngineButton
: public MutAccessUnique<WindowingState>
{
public:
    SDL_Texture* texture;
    SDL_Rect rect;
    SDL_Renderer* renderer;
    Function<void()> callback;

    EngineButton(SDL_Renderer* renderer, const char* filename, int x, int y, int w, int h) {
        auto& state = MutAccessUnique<WindowingState>::access_unique();
        this->renderer = state.renderer;
        SDL_Surface* surface = SDL_LoadBMP(filename);
        texture = SDL_CreateTextureFromSurface(state.renderer, surface);
        SDL_FreeSurface(surface);
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
    }

    void display() {
        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }

    void clicked();
};
