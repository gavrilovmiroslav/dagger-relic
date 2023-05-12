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
	, public MutAccessGroupStorage<Player, Position, Status>
	, public MutAccessGroupStorage<Pickup, Position>
	, public AllocateUnique<PickupTimer>
	, public MutAccessUnique<PickupTimer>
	, public SignalProcessor<PickupTimeoutSignal>
	, public AllocateUnique<Generator>
	, public MutAccessUnique<Generator>
	, public SignalEmitter<HealthUpdateSignal>
{
	using QueryPlayers = MutAccessGroupStorage<Player, Position, Status>;
	using QueryPickups = MutAccessGroupStorage<Pickup, Position>;
	using HealthUpdateEmitter = SignalEmitter<HealthUpdateSignal>;

	Bool on_start() override;

	void on_tick() override;

	void process_signal(PickupTimeoutSignal& signal) override;
};
