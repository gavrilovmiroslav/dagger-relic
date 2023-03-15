#pragma once

#include "Access.h"
#include "Definitions.h"
#include "DiagnosticsUtil.h"

#define MAXSAMPLES 100

using namespace containers;

class FPSCalculatorModule
	: public ecs::System
	, public SignalProcessor<core::FrameDurationSignal>
	, public SignalEmitter<DiagnosticsSignal>
{
public:
	virtual void process_signal(core::FrameDurationSignal&) override = 0;
	inline void submit_diagnostics() 
	{
		SignalEmitter<DiagnosticsSignal>::emit(DiagnosticsSignal{ DiagType::DIAG_TYPE_FPS, fps });
	}
protected:
	F32 fps;
	F32 refresh_freq = 1.0f;
};

class CountFPSCalculatorModule
	: public FPSCalculatorModule
{
public:
	void process_signal(core::FrameDurationSignal&) override;
private:
	F32 delta_sum = 0.0f;
};

class SMAFPSCalculatorModule
	: public FPSCalculatorModule
{
public:
	void process_signal(core::FrameDurationSignal&) override;
private:
	StaticArray<F32, MAXSAMPLES> duration_list;
	F32 duration_sum = 0.0f;
	U32 current_index = 0;
	F32 delta_sum = 0.0f;
};