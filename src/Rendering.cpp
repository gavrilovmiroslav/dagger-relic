#include "Rendering.h"
#include "Access.h"

#include <SDL2/SDL.h>

using namespace core;

RenderingModule::RenderingModule() 
	: render_texture{nullptr}
{
}

bool RenderingModule::on_start()
{
	return true;
}

void RenderingModule::on_tick()
{	
	SignalEmitter<RenderFrameStart>::emit(RenderFrameStart{});
	auto& state = AccessUnique<WindowingState>::access_unique();
	SDL_RenderClear(state.renderer);

	SignalEmitter<RenderSignal>::emit(RenderSignal{});
	SignalEmitter<PostRenderSignal>::emit(PostRenderSignal{});

	SDL_RenderPresent(state.renderer);
	SignalEmitter<RenderFrameEnd>::emit(RenderFrameEnd{});
}

void RenderingModule::recreate_render_surface_texture()
{
	auto& state = AccessUnique<WindowingState>::access_unique();

	I32 w, h;
	SDL_GetWindowSize(state.window, &w, &h);

	if (render_texture != nullptr)
	{
		SDL_DestroyTexture(render_texture);
	}

	render_texture = SDL_CreateTexture(state.renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888, SDL_TextureAccess::SDL_TEXTUREACCESS_STATIC, w, h);
	SDL_SetRenderTarget(state.renderer, render_texture);

	// render to screen
	SDL_SetRenderTarget(state.renderer, nullptr);
	SDL_RenderClear(state.renderer);
	SDL_RenderCopyEx(state.renderer, render_texture, nullptr, nullptr, 0, nullptr, SDL_FLIP_NONE);
	SDL_RenderPresent(state.renderer);
}

void RenderingModule::process_signal(WindowResizedSignal& signal)
{
	auto& state = AccessUnique<WindowingState>::access_unique();
	SDL_SetWindowSize(state.window, signal.width, signal.height);
	recreate_render_surface_texture();
}

void RenderingModule::on_end()
{
	if (render_texture != nullptr)
	{
		SDL_DestroyTexture(render_texture);
	}
}