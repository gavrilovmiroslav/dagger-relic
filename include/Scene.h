#pragma once
#include "Definitions.h"

struct Scene
{
    U8 boxcount;
    U8 boxcount_current;
    U8 pushplatecount;
    U8 pushplate_activenow;

    void Reset(void);
};

extern struct Scene scene;
