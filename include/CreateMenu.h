#pragma once

#include "EngineButton.h"
#include "MainMenu.h"


void createMenu()
{
    bool quit = false;

    SDL_Event event;

    EngineMenu engineMenu(nullptr, nullptr);

    engineMenu.initSplashScreen("Press Enter to start", "liberation.ttf", "data/pyramids2.bmp");

    EngineButton playButton(nullptr, "data/play1.bmp", 375, 300, 200, 50);

    EngineButton exitButton(nullptr, "data/exit1.bmp", 375, 500, 200, 50);

    while (!quit){

        SDL_PollEvent(&event);
        if(event.type==SDL_WINDOWEVENT && event.window.event==SDL_WINDOWEVENT_CLOSE){
            quit = true;
        }

        SDL_RenderClear(playButton.renderer);

        engineMenu.displaySplashScreen();
        playButton.display();
        exitButton.display();

        SDL_RenderPresent(playButton.renderer);
        SDL_Delay(20);
    }
}


