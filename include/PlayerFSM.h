#pragma once

#include "FSM.h"

struct PlayerFSM : public FSM
{ 
    PlayerFSM() : FSM("data/player.grasm") {}
};
