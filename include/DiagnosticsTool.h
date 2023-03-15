#pragma once

#include "Access.h"
#include "DiagnosticsUtil.h"
#include "Tooling.h"

using namespace std::chrono;

struct FPSData
{
	ScrollingBuffer fps_buffer;
	DiagEntry current_fps;
};

class DiagnosticsTool
	: public core::Tool
	, public SignalProcessor<core::GameStartSignal>
	, public SignalProcessor<DiagnosticsToolSignal>
{
public:
	String get_menu_tool_name() override;
	void show_tool_window() override;

private:
	void update_fps(const DiagEntry&);
	void show_fps();

	F32 time_elapsed(high_resolution_clock::time_point) const;
	void process_signal(core::GameStartSignal&) override;
	void process_signal(DiagnosticsToolSignal&) override;

	high_resolution_clock::time_point start_time;
	FPSData fps_data;
};