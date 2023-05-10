#pragma once

#include "Definitions.h"
#include "Access.h"
#include "FiniteStateMachine.h"
#include "Animations.h"
#include "SpriteRendering.h"

enum PlayerColor {
	PURPLE,
	GREEN
};

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

class PlayerFSM 
    : public FSM<PlayerTransition, PlayerState>
    , public ecs::System
    , public MutAccessComponentById<SpriteAnimation>
    , public MutAccessComponentById<Flip>
{
public:
    PlayerFSM(entt::entity id, SpriteAnimation* animation, PlayerColor color);

    entt::entity id;
    SpriteAnimation* sprite;
    PlayerColor color;

private:
    void on_exit(PlayerState fromState) override;
    void on_enter(PlayerState toState) override;

    String animationPath;
};
