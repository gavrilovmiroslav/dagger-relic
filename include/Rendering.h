#pragma once

#include "Definitions.h"
#include "Access.h"
#include "Windowing.h"

struct SDL_Texture;

namespace core
{
	struct RenderFrameStart {};
	struct RenderFrameEnd {};
	struct RenderSignal {};
	struct Render2Signal { SDL_Texture *image; uint32_t *pixels; int pitch; uint32_t w; uint32_t h; };
	struct PostRenderSignal {};

	struct WindowingState;

	class RenderingModule 
		: public ecs::System
		, public AccessUnique<WindowingState>
		, public SignalProcessor<core::WindowResizedSignal>
		, public SignalEmitter<RenderFrameStart>
		, public SignalEmitter<RenderFrameEnd>
		, public SignalEmitter<RenderSignal>
		, public SignalEmitter<Render2Signal>
		, public SignalEmitter<PostRenderSignal>
	{

	public:
		RawPtr<SDL_Texture> render_texture;
		RawPtr<SDL_Texture> render_texture2;
		I32 w, h;

		RenderingModule();

		bool on_start() override;
		void on_tick() override;
		void on_end() override;

		void recreate_render_surface_texture();
		void process_signal(core::WindowResizedSignal&) override;		
	};
}
