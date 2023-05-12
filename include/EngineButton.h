#include <iostream>
#include "SDL_ttf.h"
#include "include/window.h"
#include "MainMenu.h"
#include "SDL.h"
#include "Prelude.h"

class EngineButton {
public:
    SDL_Texture* texture;
    SDL_Rect rect;
    SDL_Renderer* renderer;
    bool clicked;
    Function<void()> callback;

    EngineButton(SDL_Renderer* renderer, const char* filename, int x, int y, int w, int h) {
        this->renderer = renderer;
        SDL_Surface* surface = SDL_LoadBMP(filename);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
        clicked = false;
    }

    void display() {
        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }

    bool isClicked(int mouseX, int mouseY) {
        if (mouseX >= rect.x && mouseX < (rect.x + rect.w) && mouseY >= rect.y && mouseY < (rect.y + rect.h)) {
            clicked = true;
            if (clicked) {
                callback();
            }
            return true;
        }
        return false;
    }

    void setCallback(std::function<void()> callback) {
        this->callback = callback;
    }
};
