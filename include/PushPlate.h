#include "Definitions.h"
#include "Access.h"

struct PushPlate
{
	U32 tid = 0;		          /* Unique ID of this PushPlate.   */
	Bool active = false;	      /* If being pressed by something. */
	Bool active_previous = false; /* Previous state of 'active'.    */
};

struct PushPlateSignal
{
	U32 tid = 0;	     /* ID of the PushPlate that was triggered. */
	Bool active = false; /* Current state of the PushPlate. */
};

struct PushPlateAllActiveSignal
{
	U32 count = 0; /* Number of PushPlates in the scene (that are also all active now). */
};

struct PushPlateCounter
{
	U32 count = 0;	      /* Number of PushPlates in the scene. */
	U32 count_target = 4; /* Number of PushPlates needed to be active for PushPlateAllActiveSignal to be emitted. */
};

/*
 * Emit PushPlateSignal when PushPlate.active changes state.
 */
class PushPlateTriggerSystem
    : public ecs::System,
      public MutAccessStorage<PushPlate>,
      public SignalEmitter<PushPlateSignal>
{
public:
	void on_tick() override;
};

/*
 * Answer to PushPlateSignal, increment PushPlateCounter.count.
 * When count_target is reached, emit PushPlateAllActiveSignal.
 */
class PushPlateCounterSystem
    : public ecs::System,
      public MutAccessUnique<PushPlateCounter>,
      public SignalEmitter<PushPlateAllActiveSignal>,
      public SignalProcessor<PushPlateSignal>
{
private:
public:
	void process_signal(PushPlateSignal &) override;
};
