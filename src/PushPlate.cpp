#include "Definitions.h"
#include "PushPlate.h"

void PushPlateTriggerSystem::on_tick(void)
{
    for (auto&& [entity, pushplate] : access_storage().each())
    {
        if (pushplate.active != pushplate.activeprevious)
        {
            pushplate.activeprevious = pushplate.active;
            SignalEmitter<PushPlateSignal>::emit({ pushplate.tid, pushplate.active });
        }
    }
}

void PushPlateCounterSystem::process_signal(PushPlateSignal&)
{
	auto& state = MutAccessUnique<PushPlateCounter>::access_unique();

    state.count++;
    if (state.count == state.count_target)
    {
        SignalEmitter<PushPlateAllActiveSignal>::emit({ state.count });
    }
}
