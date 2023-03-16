#include "TextEditorTool.h"
#include "ImguiTextEditor.h"
#include "EventBus.h"
#include <imgui.h>

#include <fstream>

TextEditorTool::TextEditorTool()
	: filepath{"<no file>"}
	, is_locked{ false }
	, extensions{ ".yaml", ".cpp", ".h", ".txt", ".lua" }
{
	editor_instance = memory::make_unique_ptr<TextEditor>();
	editor_instance->SetLanguageDefinition(TextEditor::LanguageDefinition::Plain());
}

String TextEditorTool::get_menu_tool_name()
{
	return "Text Editor";
}

String TextEditorTool::get_tool_window_appendix()
{
	return filepath;
}

void TextEditorTool::show_tool_window()
{
	auto cpos = editor_instance->GetCursorPosition();
	ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
	if (ImGui::BeginMenuBar())
	{
		if (is_locked)
		{
			if (ImGui::MenuItem("[x] Unlock"))
			{
				is_locked = false;
			}
		} 
		else
		{
			if (ImGui::MenuItem("[ ] Lock"))
			{
				is_locked = true;
			}
		}

		if (ImGui::MenuItem("Save", "", nullptr, !is_locked && filepath != "" && filepath != "<no file>"))
		{
			save_file(filepath);
		}

		if (ImGui::BeginMenu("Edit"))
		{
			bool ro = editor_instance->IsReadOnly();
			if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
				editor_instance->SetReadOnly(ro);
			ImGui::Separator();

			if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor_instance->CanUndo()))
				editor_instance->Undo();
			if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor_instance->CanRedo()))
				editor_instance->Redo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor_instance->HasSelection()))
				editor_instance->Copy();
			if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor_instance->HasSelection()))
				editor_instance->Cut();
			if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor_instance->HasSelection()))
				editor_instance->Delete();
			if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
				editor_instance->Paste();

			ImGui::Separator();

			if (ImGui::MenuItem("Select all", nullptr, nullptr))
				editor_instance->SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor_instance->GetTotalLines(), 0));

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Text("%6d/%-6d | %s | %s", 
		cpos.mLine + 1, cpos.mColumn + 1, 				
		editor_instance->GetLanguageDefinition().mName.c_str(), 
		filepath.c_str());

	editor_instance->Render("Text Editor");	
}

void TextEditorTool::process_signal(core::EditorAssetSelectedIntent& intent)
{
	if (is_locked) 
		return;

	fs::Dir entry{ intent.path };
	auto extension = entry.path().extension().string();

	if (extensions.find(extension) == extensions.end())
		return;

	filepath = intent.path;

	if (extension == ".cpp" || extension == ".h")
	{
		editor_instance->SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
	}
	else if (extension == ".yaml" || extension == ".lua")
	{
		editor_instance->SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
	}
	else
	{
		editor_instance->SetLanguageDefinition(TextEditor::LanguageDefinition::Plain());
	}

	open_file(intent.path);
}

void TextEditorTool::save_file(String filepath)
{
	if (filepath != "" && filepath != "<no file>")
	{
		std::ofstream save;
		save.open("data/" + filepath);
		if (save.is_open())
		{
			containers::DynamicArray<String> lines;
			for (auto& line : editor_instance->GetTextLines())
			{
				lines.push_back(line);
			}

			U16 row = 0;
			for (Size i = lines.size() - 1; i >= 0; i--)
			{
				if (lines[i] != "")
				{
					break;
				}

				row += 1;
			}

			row = (U16)lines.size() - row;
			for (U16 i = 0; i < row; i++)
			{
				save << lines[i] << std::endl;
			}
		}
		save.close();
	}
}

void TextEditorTool::open_file(String filepath)
{
	std::ifstream file{ "data/" + filepath };
	if (file.is_open())
	{
		String line;
		containers::DynamicArray<String> lines;
		while (std::getline(file, line))
		{
			lines.push_back(line);
		}
		while (lines.size() < 40)
		{
			lines.push_back("");
		}
		editor_instance->SetTextLines(lines);
	}

}

U32 TextEditorTool::get_flags()
{	
	return ImGuiWindowFlags_MenuBar;	
}