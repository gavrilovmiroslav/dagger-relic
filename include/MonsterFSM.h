#pragma once

#include "FSM.h"

struct MonsterFSM : public FSM
{
    MonsterFSM() : FSM("data/player.grasm") {}
};
