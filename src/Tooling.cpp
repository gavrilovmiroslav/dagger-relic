
#include "Tooling.h"
#include "Rendering.h"
#include "Windowing.h"
#include "EventBus.h"

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>
#include <implot.h>

using namespace core;

void ToolModule::on_tick()
{	
	auto& keys = KeyState::get();
	if (keys.is_down(KEY_LCTRL) && keys.is_pressed(KEY_T))
	{
		is_editor_enabled = !is_editor_enabled;
	}
}

void ToolModule::process_signal(WindowInitSignal&)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	auto& window_state = AccessUnique<WindowingState>::access_unique();
	ImGui_ImplSDL2_InitForSDLRenderer(window_state.window, window_state.renderer);
	ImGui_ImplSDLRenderer_Init(window_state.renderer);
}

void ToolModule::create_dockspace()
{
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", nullptr, window_flags);
	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}
}

void ToolModule::process_signal(PostRenderSignal&)
{
	if (is_editor_enabled)
	{
		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		create_dockspace();

		SignalEmitter<ToolRenderSignal>::emit(ToolRenderSignal{});
		auto& window_state = AccessUnique<WindowingState>::access_unique();
		
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(window_state.renderer);
	}
}

void ToolModule::process_signal(RenderFrameEnd&)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void ToolModule::process_signal(WindowShutdownSignal&)
{
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();
}

void ToolModule::process_signal(BusEventSignal& sig)
{
	if (is_editor_enabled)
	{
		ImGui_ImplSDL2_ProcessEvent(&sig.event);
	}
}

void Tool::process_signal(core::ToolRenderSignal&)
{
	if (is_visible)
	{
		auto name = get_menu_tool_name();
		auto title = get_menu_tool_name();
		const auto& appendix = get_tool_window_appendix();		
		if (appendix != "") title += " - " + appendix;
		
		ImGui::Begin((title + "###" + name).c_str(), (bool*)nullptr, get_flags());
		
		show_tool_window();
		ImGui::End();
	}
}

void Tool::process_signal(core::EditorToolMenuBarSignal&)
{
	if (ImGui::MenuItem((String(is_visible ? "[+] " : "[ ] ") + get_menu_tool_name()).c_str()))
	{
		is_visible = !is_visible;
	}
}

U32 Tool::get_flags()
{
	return ImGuiWindowFlags_None;
}
