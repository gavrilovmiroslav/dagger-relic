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

	virtual void process_signal(core::FrameDurationSignal& signal) override = 0;
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

class PickupSpawnTimer
	: public Timer<PickupSpawnSignal>
{
public:
	PickupSpawnTimer();
	void process_signal(core::FrameDurationSignal&) override;
};
