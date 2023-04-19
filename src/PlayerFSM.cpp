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

PlayerFSM::PlayerFSM(entt::entity entity)
    : id(entity)
{
    currentState = PlayerState::AIRBORNE;

    sprite = MutAccessComponentById<SpriteAnimation>::get(id);

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
            sprite->change_to("test/WizardRun");
            //std::cout << "changed animation to RUN" << std::endl;
            break;
        }
        case PlayerState::IDLE:
        {
            sprite->change_to("test/WizardIdle");
            //std::cout << "changed animation to IDLE" << std::endl;
            break;
        }
        default:
            break;
    }
    
}