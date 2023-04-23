#pragma once

#include "Access.h"
#include "Definitions.h"
#include "GameSpecificComponents.h"

template<typename ConcreteSignal>
class Timer
	: public ecs::System
	, public SignalProcessor<core::FrameDurationSignal>
	, public SignalEmitter<ConcreteSignal>
{
public:
	Timer(F32 time_limit)
		: time_limit(time_limit)
	{}

	void process_signal(core::FrameDurationSignal& signal)
	{
		time += signal.frame_duration;
		if (time >= time_limit)
		{
			on_timer_trigger();
			reset_timer();
		}
	}
	void on_timer_trigger()
	{
		SignalEmitter<ConcreteSignal>::emit(ConcreteSignal{});
	}
	void reset_timer()
	{
		time = 0.0f;
	}
protected:
	F32 time = 0.0f;
	F32 time_limit;
};
