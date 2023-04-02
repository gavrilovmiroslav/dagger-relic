#include "Rendering.h"
#include "Access.h"
#include "FPNG.h"

#include <SDL.h>

using namespace core;

RenderingModule::RenderingModule() 
	: render_texture{nullptr}, render_texture2{nullptr}
{
}

bool RenderingModule::on_start()
{
	fpng::fpng_init();
	return true;
}

void RenderingModule::on_tick()
{
	uint32_t *texture2pixels;
	int       texture2pitch;

	SignalEmitter<RenderFrameStart>::emit(RenderFrameStart{});
	auto& state = AccessUnique<WindowingState>::access_unique();
	SDL_RenderClear(state.renderer);

	SignalEmitter<RenderSignal>::emit(RenderSignal{});
	SignalEmitter<PostRenderSignal>::emit(PostRenderSignal{});
	SDL_LockTexture(render_texture2, nullptr, (void **) &texture2pixels, &texture2pitch);
	memset(texture2pixels, 0, h * texture2pitch);
	SignalEmitter<Render2Signal>::emit(Render2Signal{ render_texture2, texture2pixels, texture2pitch, (uint32_t) w, (uint32_t) h });
	SDL_UnlockTexture(render_texture2);

	SDL_SetTextureBlendMode(render_texture2, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	SDL_RenderCopyEx(state.renderer, render_texture2, nullptr, nullptr, 0, nullptr, SDL_FLIP_NONE);

	SDL_RenderPresent(state.renderer);
	SignalEmitter<RenderFrameEnd>::emit(RenderFrameEnd{});
}

void RenderingModule::recreate_render_surface_texture()
{
	auto& state = AccessUnique<WindowingState>::access_unique();

	I32 w, h;

	SDL_GetWindowSize(state.window, &w, &h);

	this->w = w;
	this->h = h;

	if (render_texture != nullptr)
	{
		SDL_DestroyTexture(render_texture);
	}

	render_texture = SDL_CreateTexture(state.renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888, SDL_TextureAccess::SDL_TEXTUREACCESS_STATIC, w, h);
	SDL_SetRenderTarget(state.renderer, render_texture);

	if (render_texture2 != nullptr)
	{
		SDL_DestroyTexture(render_texture2);
	}
	render_texture2 = SDL_CreateTexture(state.renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888, SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING, w, h);

	// render to screen
	SDL_SetRenderTarget(state.renderer, nullptr);
	SDL_RenderClear(state.renderer);
	SDL_RenderCopyEx(state.renderer, render_texture, nullptr, nullptr, 0, nullptr, SDL_FLIP_NONE);
	SDL_RenderCopyEx(state.renderer, render_texture2, nullptr, nullptr, 0, nullptr, SDL_FLIP_NONE);
	SDL_RenderPresent(state.renderer);

}

void RenderingModule::process_signal(WindowResizedSignal& signal)
{
	auto& state = AccessUnique<WindowingState>::access_unique();
	SDL_SetWindowSize(state.window, signal.width, signal.height);
	this->w = signal.width;
	this->h = signal.height;
	recreate_render_surface_texture();
}

void RenderingModule::on_end()
{
	if (render_texture != nullptr)
	{
		SDL_DestroyTexture(render_texture);
	}
	if (render_texture2 != nullptr)
	{
		SDL_DestroyTexture(render_texture);
	}
}