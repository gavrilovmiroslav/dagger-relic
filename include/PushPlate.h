#include "Definitions.h"

struct PushPlate
{
    bool     active = false; /* If being pressed by something.      */
    uint32_t tid    = 0;     /* Unique ID of thing to trigger or 0. */
};
