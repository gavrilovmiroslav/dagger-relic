#pragma once

#include "Definitions.h"
#include "Access.h"
#include "FiniteStateMachine.h"
#include "Animations.h"
#include "SpriteRendering.h"

enum PlayerTransition
{
    STAND,
    RUN,
    WHIP
};

enum PlayerState
{
    STANDING,
    RUNNING,
    WHIPPING
};

class PlayerFSM
    : public FSM<PlayerTransition, PlayerState>
    , public ecs::System
    , public MutAccessComponentById<SpriteAnimation>
    , public MutAccessComponentById<Flip>
{
public:
    PlayerFSM(entt::entity id);

    entt::entity id;
    SpriteAnimation* sprite;

private:
    void on_exit(PlayerState fromState) override;
    void on_enter(PlayerState toState) override;
};
