#pragma once

#include "Access.h"
#include "Definitions.h"
#include "GameSpecificComponents.h"

template<int TimeLimit, typename TimeoutSignal>
class Timer final : public SignalProcessor<core::FrameDurationSignal>
{
	inline static constexpr const F32 TimeLimitMs = (F32)TimeLimit / 1000.0f;

public:
	Timer() {}

	void process_signal(core::FrameDurationSignal& signal)
	{
		if (is_started)
		{
			time += signal.frame_duration;
			if (time >= TimeLimitMs)
			{
				SignalEmitter<TimeoutSignal> emitter;
				emitter.emit(TimeoutSignal{});
				time = 0.0f;
			}
		}
	}

	void start()
	{
		time = 0.0f;
		is_started = true;
	}

	void stop()
	{
		is_started = false;
	}

protected:
	Bool is_started = false;
	F32 time = 0.0f;
};
