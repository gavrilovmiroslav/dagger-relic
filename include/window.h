#ifndef WINDOW_H
#define WINDOW_H

#include <SDL.h>

class EngineWindow {

    public: 
        SDL_Window *mainWindow = NULL;
        SDL_Surface *screenSurface = NULL;
        SDL_Renderer *renderer = NULL;

        void createWindow(const char *windowTitle, const int width, const int height){
            mainWindow = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
            renderer = SDL_CreateRenderer(mainWindow, -1, 0);
            screenSurface = SDL_GetWindowSurface(mainWindow);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        }

        void destroyWindow(){
            SDL_UpdateWindowSurface(mainWindow);
            SDL_DestroyWindow(mainWindow);
            SDL_Quit();            
        }


};

#endif