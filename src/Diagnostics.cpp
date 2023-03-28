#include "Access.h"
#include "Diagnostics.h"

const DiagEntry& DiagnosticsModule::get_diagnostics(DiagType diagType)
{
	assert(diagType != DIAG_TYPE_COUNT);
	return diagnostics[diagType];
}

void DiagnosticsModule::set_diagnostics(DiagType diagType, F32 value)
{
	assert(diagType != DIAG_TYPE_COUNT);

	DiagEntry& entry = diagnostics[diagType];
	entry.value = value;
	entry.sample_time = high_resolution_clock::now();
}

void DiagnosticsModule::process_signal(DiagnosticsSignal& signal)
{
	set_diagnostics(signal.diagType, signal.value);
}