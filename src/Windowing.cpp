#include "Windowing.h"

#include "Config.h"
#include "Engine.h"
#include "Logger.h"

#include <SDL.h>

using namespace core;

bool WindowingModule::on_start()
{
	auto& state = MutAccessUnique<WindowingState>::access_unique();
	auto& get_config = Engine::get_instance().get_config();
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		Logger::error("Couldn't initialize SDL: {}.", SDL_GetError());
		return false;
	}

	int width = std::atoi(get_config["window"]["width"].c_str());
	int height = std::atoi(get_config["window"]["height"].c_str());
	state.window = SDL_CreateWindow(
		get_config["window"]["title"].c_str(),
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height, SDL_WINDOW_SHOWN);

	if (state.window == nullptr)
	{
		Logger::error("SDL window couldn't be created: {}.", SDL_GetError());
		return false;
	}

	state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_ACCELERATED);	
	SignalEmitter<WindowInitSignal>::emit(WindowInitSignal{});

	int w, h;
	SDL_GetWindowSize(state.window, &w, &h);
	SignalEmitter<WindowResizedSignal>::emit(WindowResizedSignal{ (U32)w, (U32)h });

	Logger::info("Backend initialized correctly.");
	return true;
}

void WindowingModule::on_end()
{
	SignalEmitter<WindowShutdownSignal>::emit(WindowShutdownSignal{});

	auto& state = MutAccessUnique<WindowingState>::access_unique();

	if (state.renderer)
	{
		SDL_DestroyRenderer(state.renderer);
		Logger::info("Renderer destroyed successfully.");
	}

	if (state.window)
	{
		SDL_DestroyWindow(state.window);
		Logger::info("Window destroyed successfully.");
	}
	
	// TODO: SDL_Quit hangs! Issue likely reported: https://github.com/libsdl-org/SDL/issues/4276
	//SDL_Quit();
	Logger::info("Quitting.");
}