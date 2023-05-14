#pragma once
#include "Definitions.h"

struct Scene
{
    U8 boxcount;
    U8 boxcount_current;
    U8 pushplatecount;
    U8 pushplate_activenow;

    Bool dooropen = false;
    I32  doorx    = 0;
    I32  doory    = 0;

    ecs::Entity player;

    std::vector<ecs::Entity> entity;

    void Reset(void);
};

extern struct Scene scene;
