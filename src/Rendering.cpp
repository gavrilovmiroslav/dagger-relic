#include "Rendering.h"
#include "Access.h"

#include <SDL.h>

using namespace core;

RenderingModule::RenderingModule()
	: render_texture{nullptr}
	, post_render_texture{nullptr}
	, post_render_texture2{nullptr}
{
}

bool RenderingModule::on_start()
{
	return true;
}

void RenderingModule::on_tick()
{
	U32 *postprocess_pixels;
	U32 *postprocess_pixels2;
	I32  postprocess_pitch;
	I32  postprocess_pitch2;

	SignalEmitter<RenderFrameStart>::emit(RenderFrameStart{});
	auto& state = AccessUnique<WindowingState>::access_unique();
	SDL_RenderClear(state.renderer);

	SignalEmitter<RenderSignal>::emit(RenderSignal{});
	SignalEmitter<PostRenderSignal>::emit(PostRenderSignal{});
	SDL_LockTexture(post_render_texture, nullptr, (void **) &postprocess_pixels, &postprocess_pitch);
	memset(postprocess_pixels, 0, height * postprocess_pitch);
	SignalEmitter<PostProcessRenderSignal>::emit(PostProcessRenderSignal{postprocess_pixels, postprocess_pitch, (U32) width, (U32) height });
	SDL_UnlockTexture(post_render_texture);

	SDL_LockTexture(post_render_texture2, nullptr, (void **) &postprocess_pixels2, &postprocess_pitch2);
	memset(postprocess_pixels2, 0, height * postprocess_pitch2);
	SignalEmitter<PostProcessRenderSignal2>::emit(PostProcessRenderSignal2{postprocess_pixels2, postprocess_pitch, (U32) width, (U32) height });
	SDL_UnlockTexture(post_render_texture2);

	SDL_SetTextureBlendMode(post_render_texture, SDL_BlendMode::SDL_BLENDMODE_MOD);
	SDL_RenderCopyEx(state.renderer, post_render_texture, nullptr, nullptr, 0, nullptr, SDL_FLIP_NONE);

	SDL_SetTextureBlendMode(post_render_texture2, SDL_BlendMode::SDL_BLENDMODE_ADD);
	SDL_RenderCopyEx(state.renderer, post_render_texture2, nullptr, nullptr, 0, nullptr, SDL_FLIP_NONE);

	SDL_RenderPresent(state.renderer);
	SignalEmitter<RenderFrameEnd>::emit(RenderFrameEnd{});
}

void RenderingModule::recreate_render_surface_texture()
{
	auto& state = AccessUnique<WindowingState>::access_unique();

	I32 w, h;

	SDL_GetWindowSize(state.window, &w, &h);

	this->width  = w;
	this->height = h;

	if (render_texture != nullptr)
	{
		SDL_DestroyTexture(render_texture);
	}

	render_texture = SDL_CreateTexture(state.renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888, SDL_TextureAccess::SDL_TEXTUREACCESS_STATIC, w, h);
	SDL_SetRenderTarget(state.renderer, render_texture);

	if (post_render_texture != nullptr)
	{
		SDL_DestroyTexture(post_render_texture);
	}
	post_render_texture = SDL_CreateTexture(state.renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888, SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING, w, h);

	if (post_render_texture2 != nullptr)
	{
		SDL_DestroyTexture(post_render_texture2);
	}
	post_render_texture2 = SDL_CreateTexture(state.renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888, SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING, w, h);

	// render to screen
	SDL_SetRenderTarget(state.renderer, nullptr);
	SDL_RenderClear(state.renderer);
	SDL_RenderCopyEx(state.renderer, render_texture, nullptr, nullptr, 0, nullptr, SDL_FLIP_NONE);
	SDL_RenderCopyEx(state.renderer, post_render_texture, nullptr, nullptr, 0, nullptr, SDL_FLIP_NONE);
	SDL_RenderCopyEx(state.renderer, post_render_texture2, nullptr, nullptr, 0, nullptr, SDL_FLIP_NONE);
	SDL_RenderPresent(state.renderer);

}

void RenderingModule::process_signal(WindowResizedSignal& signal)
{
	auto& state = AccessUnique<WindowingState>::access_unique();
	SDL_SetWindowSize(state.window, signal.width, signal.height);
	this->width  = signal.width;
	this->height = signal.height;
	recreate_render_surface_texture();
}

void RenderingModule::on_end()
{
	if (render_texture != nullptr)
	{
		SDL_DestroyTexture(render_texture);
	}

	if (post_render_texture != nullptr)
	{
		SDL_DestroyTexture(post_render_texture);
	}

	if (post_render_texture2 != nullptr)
	{
		SDL_DestroyTexture(post_render_texture2);
	}
}
