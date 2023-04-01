#include "Definitions.h"
#include "Setup.h"

struct TriggerEvent
{
    tid target = 0; /* What should be triggered. */
    tid source = 0; /* Who executed the trigger. */
};

struct Trigger
{
    tid target[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; /* Unique IDs of things to trigger or 0. */

    inline void SendEvents(void)
    {
        for (int i = 0; i < 8; i++)
        {
            Engine::get_instance().dispatcher.trigger<TriggerEvent>(target[i]);
        }
    }
};
