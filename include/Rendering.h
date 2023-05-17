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
	struct PostRenderSignal {};
	struct PostProcessRenderSignal { U32 *pixels; I32 pitch; U32 w; U32 h; };
	struct PostProcessRenderSignal2 { U32 *pixels; I32 pitch; U32 w; U32 h; };

	struct WindowingState;

	class RenderingModule
		: public ecs::System
		, public AccessUnique<WindowingState>
		, public SignalProcessor<core::WindowResizedSignal>
		, public SignalEmitter<RenderFrameStart>
		, public SignalEmitter<RenderFrameEnd>
		, public SignalEmitter<RenderSignal>
		, public SignalEmitter<PostProcessRenderSignal>
		, public SignalEmitter<PostProcessRenderSignal2>
		, public SignalEmitter<PostRenderSignal>
	{

	public:
		RawPtr<SDL_Texture> render_texture;
		RawPtr<SDL_Texture> post_render_texture;
		RawPtr<SDL_Texture> post_render_texture2;
		I32 width, height;

		RenderingModule();

		bool on_start() override;
		void on_tick() override;
		void on_end() override;

		void recreate_render_surface_texture();
		void process_signal(core::WindowResizedSignal&) override;
	};
}
