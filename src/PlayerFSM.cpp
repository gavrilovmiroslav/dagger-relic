#include "PlayerFSM.h"

#include <iostream>

std::string stateName(PlayerState state) {
    switch(state) {
        case RUNNING: return "RUNNING"; break;
        case STANDING: return "STANDING"; break;
        case WHIPPING: return "WHIPPING"; break;
    }
}

PlayerFSM::PlayerFSM(entt::entity entity)
    : id(entity)
{
    currentState = PlayerState::STANDING;

    sprite = MutAccessComponentById<SpriteAnimation >::get(id);

    add_transition_to_new_state(PlayerTransition::STAND, PlayerState::STANDING);
    add_transition_to_new_state(PlayerTransition::RUN, PlayerState::RUNNING);
    add_transition_to_new_state(PlayerTransition::WHIP, PlayerState::WHIPPING);

    add_valid_transitions_from_state( PlayerState::STANDING,
        containers::Set<PlayerTransition> { PlayerTransition::RUN, PlayerTransition::WHIP });
    add_valid_transitions_from_state( PlayerState::RUNNING,
        containers::Set<PlayerTransition> { PlayerTransition::STAND, PlayerTransition::WHIP });
    add_valid_transitions_from_state( PlayerState::WHIPPING,
        containers::Set<PlayerTransition> { PlayerTransition::STAND, PlayerTransition::RUN });
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
	case PlayerState::WHIPPING:
	{
	    sprite->change_to("pyramidplunder/archaeologist_whipping");
            std::cout << "changed animation to WHIP" << std::endl;
            break;
	}
        default:
            break;
    }

}
