#pragma once

#include "Access.h"
#include "Definitions.h"
#include "DiagnosticsUtil.h"

using namespace containers;
using namespace std::chrono;

class DiagnosticsModule
	: public ecs::System
	, public SignalProcessor<DiagnosticsSignal>
	, public SignalEmitter<DiagnosticsToolSignal>
{

public:
	const DiagEntry& get_diagnostics(DiagType diagType);
	void set_diagnostics(DiagType diagType, F32 value);

private:
	StaticArray<DiagEntry, DIAG_TYPE_COUNT> diagnostics;
	void process_signal(DiagnosticsSignal&) override;
};

