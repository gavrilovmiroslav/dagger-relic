#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_ttf.h>

using namespace std;

class EngineMenu
: public MutAccessUnique<WindowingState>
{
    public:

        EngineMenu(SDL_Renderer *windowsplashFontRenderer, SDL_Window *window){
            auto& state = MutAccessUnique<WindowingState>::access_unique();
            TTF_Init();
            splashFontRenderer = state.renderer;
            windowScreen = state.window;
            splashScreenTextBlink = 0;
            SDL_GetWindowSize(windowScreen, &windowWidth, &windowHeight);
        }

        void initSplashScreen(const char *displayName, const char *fontFamily, const char *backgroundImg = NULL){
           prepareSplashScreen(displayName, fontFamily, backgroundImg);
           
        }

        void displaySplashScreen(){
            renderSplashScreen();
        }

        void quitSplashScreen(){
            destroySplashScreen();
        }

    private:
        SDL_Window *windowScreen;
        SDL_Renderer *splashFontRenderer, *splashImageRenderer;
        SDL_Texture *splashFontTexture, *splashImageTexture;
        SDL_Surface *splashFontSurface;

        TTF_Font *splashFont;
        const char *splashText;

        int windowHeight, windowWidth, splashScreenTextBlink;


        void prepareSplashScreen(const char *displayName, const char *fontFamily, const char *backgroundImg){
            splashFont = TTF_OpenFont(fontFamily, 25);
            splashText = displayName;

            if(backgroundImg != NULL){
                
                splashImageRenderer = SDL_CreateRenderer(windowScreen, -1, 0);
                SDL_Surface *splashBackgroundSurface = SDL_LoadBMP(backgroundImg);
                splashImageTexture = SDL_CreateTextureFromSurface(splashFontRenderer, splashBackgroundSurface);

            }
        }


        void renderSplashScreenText(){
            int textWidth = 0;
            int textHeight = 0;

            SDL_Color color = {255, 255, 255};
            splashFontSurface = TTF_RenderText_Solid(splashFont, splashText, color);
            splashFontTexture = SDL_CreateTextureFromSurface(splashFontRenderer, splashFontSurface);

            splashScreenTextBlink++;
            if(splashScreenTextBlink == 100){

                splashScreenTextBlink = 0;
                
            }

            //Handle splash screen text blinkg effect
            if(splashScreenTextBlink < 50){

                SDL_QueryTexture(splashFontTexture, NULL, NULL, &textWidth, &textHeight);

                const int devideScreenWidth = windowWidth / 2;
                const int deviceText = textWidth / 2;
                const int dediceScreenHeight = windowHeight / 2;

                SDL_Rect fontStruct = {devideScreenWidth - deviceText, dediceScreenHeight, textWidth, textHeight };                
                SDL_RenderCopy(splashFontRenderer, splashFontTexture, NULL, &fontStruct);

            }else{

                splashFontTexture = NULL;
                splashFontSurface = NULL;

            }
        }

        void renderSplashBackground(){
            SDL_RenderCopy(splashFontRenderer, splashImageTexture, NULL, NULL);
            SDL_RenderPresent(splashFontRenderer);
        }

        void renderSplashScreen(){
            renderSplashBackground();
            renderSplashScreenText();
            

        }

        void destroySplashScreen(){

            SDL_DestroyTexture(splashFontTexture);
            SDL_FreeSurface(splashFontSurface);

        }

};

#endif