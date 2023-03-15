#include "Engine.h"

#include "Access.h"

struct Game
	: public ecs::LifecycleTrait
	, public SignalProcessor<core::GameStartSignal>
	, public SignalProcessor<core::GameEndSignal>
{
	virtual void process_signal(core::GameStartSignal&) = 0;
	virtual void process_signal(core::GameEndSignal&) = 0;
};

