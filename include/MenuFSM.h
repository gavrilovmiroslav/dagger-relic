#pragma once

#include "Definitions.h"
#include "Access.h"
#include "FiniteStateMachine.h"
#include "Animations.h"
#include "SpriteRendering.h"

enum MenuTransition
{
    PAUSE,
    EXIT,
    PLAY
};

enum MenuState
{
    PAUSING,
    EXITING,
    PLAYING
};

class MenuFSM
    : public FSM<MenuTransition, MenuState>
    , public ecs::System
    , public MutAccessComponentById<SpriteAnimation>
    , public MutAccessComponentById<Flip>
{
public:
    MenuFSM(entt::entity id);

    entt::entity id;

private:
    void on_exit(MenuState fromState) override;
    void on_enter(MenuState toState) override;
};
