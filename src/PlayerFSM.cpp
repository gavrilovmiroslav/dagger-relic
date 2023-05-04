#include "PlayerFSM.h"

// just for debugging purposes
#include <iostream>
std::string stateName(PlayerState state) {
    switch(state) {
        case RUNNING: return "RUNNING"; break;
        case STANDING: return "STANDING"; break;
    }
}

PlayerFSM::PlayerFSM(entt::entity entity)
    : id(entity)
{
    currentState = PlayerState::STANDING;

    sprite = MutAccessComponentById<SpriteAnimation >::get(id);

    add_transition_to_new_state(PlayerTransition::STAND, PlayerState::STANDING);
    add_transition_to_new_state(PlayerTransition::RUN, PlayerState::RUNNING);

    add_valid_transitions_from_state( PlayerState::STANDING,
        containers::Set<PlayerTransition> { PlayerTransition::RUN });
    add_valid_transitions_from_state( PlayerState::RUNNING,
        containers::Set<PlayerTransition> { PlayerTransition::STAND });
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
        case PlayerState::STANDING:
        {
            sprite->change_to("pyramidplunder/archaeologist_standing");
            std::cout << "changed animation to STAND" << std::endl;
            break;
        }
        case PlayerState::RUNNING:
        {
            sprite->change_to("pyramidplunder/archaeologist_running");
            std::cout << "changed animation to RUN" << std::endl;
            break;
        }
        default:
            break;
    }

}
