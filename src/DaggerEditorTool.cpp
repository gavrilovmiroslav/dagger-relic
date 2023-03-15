#include "DaggerEditorTool.h"
#include "EventBus.h"
#include <imgui.h>

void DaggerEditorTool::process_signal(core::ToolRenderSignal&)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Dagger"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				core::Engine::get_instance().quit();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Assets"))
		{
			SignalEmitter<core::EditorToolAssetLoadingSignal>::emit(core::EditorToolAssetLoadingSignal{});
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows"))
		{
			SignalEmitter<core::EditorToolMenuBarSignal>::emit(core::EditorToolMenuBarSignal{});
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}
