#pragma once

#include "Config.h"
#include "Definitions.h"
#include "Engine.h"
#include "Logger.h"
#include "Access.h"

using namespace memory;

namespace core
{
	struct PostRenderSignal;
	struct WindowingState;
	struct WindowInitSignal;
	struct WindowShutdownSignal;
	struct RenderFrameEnd;
	struct BusEventSignal;

	struct ToolRenderSignal {};

	struct EditorAssetSelectedIntent 
	{
		String path;
	};

	struct EditorToolAssetLoadingSignal {};
	struct EditorToolMenuBarSignal {};

	class ToolModule
		: public ecs::System
		, public AccessUnique<WindowingState>
		, public SignalProcessor<PostRenderSignal>
		, public SignalProcessor<WindowInitSignal>
		, public SignalProcessor<WindowShutdownSignal>
		, public SignalProcessor<RenderFrameEnd>
		, public SignalProcessor<BusEventSignal>
		, public SignalEmitter<ToolRenderSignal>
	{		
		void on_tick() override;

		void process_signal(WindowInitSignal&) override;
		void process_signal(WindowShutdownSignal&) override;
		void process_signal(PostRenderSignal&) override;
		void process_signal(BusEventSignal&) override;
		void process_signal(RenderFrameEnd&) override;

		void create_dockspace();
	
	public:
		Bool is_editor_enabled = true;
	};

	struct Tool
		: public ecs::System
		, public SignalProcessor<core::ToolRenderSignal>
		, public SignalProcessor<core::EditorToolMenuBarSignal>
	{
		virtual U32 get_flags();

		virtual String get_menu_tool_name() = 0;
		virtual String get_tool_window_appendix() { return ""; }
		virtual void show_tool_window() = 0;

		void process_signal(core::ToolRenderSignal&) override;
		void process_signal(core::EditorToolMenuBarSignal&) override;

		Bool is_visible;

		Tool()
			: is_visible{ false }
		{}

		Bool is_tool() override { return true; }
	};
}
