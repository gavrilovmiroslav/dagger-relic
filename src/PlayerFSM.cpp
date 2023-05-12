#include "PlayerFSM.h"

// just for debugging purposes
#include <iostream>
std::string stateName(PlayerState state) {
    switch(state) {
        case WALKING: return "WALKING"; break;
        case IDLE: return "IDLE"; break;
        case ATTACKING: return "ATTACKING"; break;
        case AIRBORNE: return "AIRBORNE"; break;
    }
}

PlayerFSM::PlayerFSM(entt::entity entity, SpriteAnimation* animation, PlayerColor color = PlayerColor::PURPLE)
    : id(entity)
    , sprite(animation)
    , color(color)
{
    currentState = PlayerState::AIRBORNE;
    animationPath = (color == PlayerColor::PURPLE) ? "test/Wizard1/" : "test/Wizard2/";

    add_transition_to_new_state(PlayerTransition::STOP, PlayerState::IDLE);
    add_transition_to_new_state(PlayerTransition::WALK, PlayerState::WALKING);
    add_transition_to_new_state(PlayerTransition::ATTACK, PlayerState::ATTACKING);
    add_transition_to_new_state(PlayerTransition::JUMP, PlayerState::AIRBORNE);
    add_transition_to_new_state(PlayerTransition::FALL, PlayerState::AIRBORNE);

    add_valid_transitions_from_state( PlayerState::IDLE, 
        containers::Set<PlayerTransition> { PlayerTransition::ATTACK, PlayerTransition::JUMP, PlayerTransition::WALK, PlayerTransition::FALL });
    add_valid_transitions_from_state( PlayerState::WALKING, 
        containers::Set<PlayerTransition> { PlayerTransition::ATTACK, PlayerTransition::JUMP, PlayerTransition::STOP, PlayerTransition::FALL });
    add_valid_transitions_from_state( PlayerState::ATTACKING, 
        containers::Set<PlayerTransition> { PlayerTransition::STOP });
    add_valid_transitions_from_state( PlayerState::AIRBORNE, 
        containers::Set<PlayerTransition> { PlayerTransition::ATTACK, PlayerTransition::STOP });
}

void PlayerFSM::on_exit(PlayerState fromState)
{
    std::cout << "exiting state: [" << stateName(fromState) << "], ";
}

void PlayerFSM::on_enter(PlayerState toState)
{
    std::cout << "entering state: [" << stateName(toState) << "]" << std::endl;

    switch (toState)
    {
        case PlayerState::WALKING:
        {
            sprite->change_to(animationPath + "WizardRun");
            //std::cout << "changed animation to RUN" << std::endl;
            break;
        }
        case PlayerState::IDLE:
        {
            sprite->change_to(animationPath + "WizardIdle");
            //std::cout << "changed animation to IDLE" << std::endl;
            break;
        }
        case PlayerState::AIRBORNE:
        {
            sprite->change_to(animationPath + "WizardAirborne");
            break;
        }
        default:
            break;
    }
    
}