#pragma once

#include "FiniteStateMachine.h"

enum PlayerTransition
{
    JUMP,
    WALK,
    STOP,
    ATTACK,
    FALL
};

enum PlayerState
{
    IDLE,
    WALKING,
    AIRBORNE,
    ATTACKING
};

class PlayerFSM : public FSM<PlayerTransition, PlayerState>
{
public:
    PlayerFSM();

private:
    void on_exit(PlayerState fromState) override;
    void on_enter(PlayerState toState) override;
};
