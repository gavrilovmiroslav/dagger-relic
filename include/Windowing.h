#pragma once

#include "Definitions.h"
#include "Access.h"

using namespace memory;

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Surface;

namespace core
{
	struct WindowInitSignal {};
	struct WindowShutdownSignal {};

	struct WindowResizedSignal
	{
		U32 width;
		U32 height;
	};

	struct WindowingState
	{
		RawPtr<SDL_Window> window;
		RawPtr<SDL_Renderer> renderer;
	};

	class WindowingModule
		: public ecs::System
		, public AllocateUnique<WindowingState>
		, public MutAccessUnique<WindowingState>
		, public SignalEmitter<WindowInitSignal>
		, public SignalEmitter<WindowShutdownSignal>
		, public SignalEmitter<WindowResizedSignal>
	{
	public:
		bool on_start() override;

		void on_end() override;
	};
}
