#include "Timer.h"
#include "GameSpecificComponents.h"

PickupSpawnTimer::PickupSpawnTimer()
	: Timer(500.0f)
{}

void PickupSpawnTimer::process_signal(core::FrameDurationSignal& signal)
{
	time += signal.frame_duration;
	if (time >= time_limit)
	{
		on_timer_trigger();
		reset_timer();
	}
}
