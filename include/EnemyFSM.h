#pragma once

#include "FSM.h"

struct EnemyFSM : public FSM
{ 
    EnemyFSM() : FSM("data/player.grasm") {}
};
