#include "Access.h"
#include "DiagnosticsTool.h"
#include "Diagnostics.h"

#include <imgui.h>

using namespace std::chrono;

String DiagnosticsTool::get_menu_tool_name()
{
	return "Diagnostics";
}

void DiagnosticsTool::show_tool_window()
{
	show_fps();
}

void DiagnosticsTool::process_signal(DiagnosticsToolSignal& signal)
{
	switch (signal.diagType)
	{
	case DIAG_TYPE_FPS:
		update_fps(signal.diagEntry);
	}
}

void DiagnosticsTool::update_fps(const DiagEntry& entry)
{
	fps_data.current_fps = entry;
	fps_data.fps_buffer.AddPoint(time_elapsed(fps_data.current_fps.sample_time), fps_data.current_fps.value);
}

void DiagnosticsTool::show_fps()
{
	const DiagEntry& fps = fps_data.current_fps;
	const ScrollingBuffer& fps_buffer = fps_data.fps_buffer;

	ImGui::Text("FPS: %f", fps.value);
}

void DiagnosticsTool::process_signal(core::GameStartSignal& signal)
{
	start_time = high_resolution_clock::now();
	fps_data.fps_buffer.AddPoint(time_elapsed(start_time), 0);
}

F32 DiagnosticsTool::time_elapsed(high_resolution_clock::time_point tp) const
{
	duration<F32> dur = tp - start_time;
	return dur.count();
}