#pragma once

#include "Definitions.h"
#include "Access.h"
#include "Timer.h"
#include "GameSpecificComponents.h"
#include "Generator.h"

struct PickupTimeoutSignal {};

using PickupTimer = Timer<3000, PickupTimeoutSignal>;

struct PickupSystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, Position>
	, public MutAccessGroupStorage<Pickup, Position>
	, public AllocateUnique<PickupTimer>
	, public MutAccessUnique<PickupTimer>
	, public SignalProcessor<PickupTimeoutSignal>
	, public AllocateUnique<Generator>
	, public MutAccessUnique<Generator>
{
	using QueryPlayers = MutAccessGroupStorage<Player, Position>;
	using QueryPickups = MutAccessGroupStorage<Pickup, Position>;

	Bool on_start() override;

	void on_tick() override;

	void process_signal(PickupTimeoutSignal& signal) override;
};
