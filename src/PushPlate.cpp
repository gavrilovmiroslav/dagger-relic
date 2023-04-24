#include "Definitions.h"
#include "PushPlate.h"

void PushPlateTriggerSystem::on_tick(void)
{
	for (auto &&[entity, push_plate] : access_storage().each())
	{
		if (push_plate.active != push_plate.active_previous)
		{
			push_plate.active_previous = push_plate.active;
			SignalEmitter<PushPlateSignal>::emit({push_plate.tid, push_plate.active});
		}
	}
}

void PushPlateCounterSystem::process_signal(PushPlateSignal &)
{
	auto &state = MutAccessUnique<PushPlateCounter>::access_unique();

	state.count++;
	if (state.count == state.count_target)
	{
		SignalEmitter<PushPlateAllActiveSignal>::emit({state.count});
	}
}
