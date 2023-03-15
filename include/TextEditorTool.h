#pragma once

#include "Access.h"
#include "Definitions.h"
#include "Tooling.h"

#include "ImguiTextEditor.h"

struct TextEditorTool
	: core::Tool
	, public SignalProcessor<core::EditorAssetSelectedIntent>
{
	containers::Set<String> extensions;
	memory::UniquePtr<TextEditor> editor_instance;
	String filepath;
	Bool is_locked;

	TextEditorTool();

	void process_signal(core::EditorAssetSelectedIntent&) override;	

	String get_menu_tool_name() override;
	String get_tool_window_appendix() override;
	void show_tool_window() override;

	U32 get_flags() override;

	void open_file(String filepath);
	void save_file(String filepath);
};