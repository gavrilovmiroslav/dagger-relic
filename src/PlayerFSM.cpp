#include "PlayerFSM.h"

PlayerFSM::PlayerFSM()
{
    currentState = PlayerState::IDLE;

    add_transition_to_new_state(PlayerTransition::STOP, PlayerState::IDLE);
    add_transition_to_new_state(PlayerTransition::WALK, PlayerState::WALKING);
    add_transition_to_new_state(PlayerTransition::ATTACK, PlayerState::ATTACKING);
    add_transition_to_new_state(PlayerTransition::JUMP, PlayerState::AIRBORNE);

    add_valid_transitions_from_state( PlayerState::IDLE, 
        containers::Set<PlayerTransition> { PlayerTransition::ATTACK, PlayerTransition::JUMP, PlayerTransition::WALK });
    add_valid_transitions_from_state( PlayerState::WALKING, 
        containers::Set<PlayerTransition> { PlayerTransition::ATTACK, PlayerTransition::JUMP, PlayerTransition::STOP });
    add_valid_transitions_from_state( PlayerState::ATTACKING, 
        containers::Set<PlayerTransition> { PlayerTransition::STOP });
    add_valid_transitions_from_state( PlayerState::AIRBORNE, 
        containers::Set<PlayerTransition> { PlayerTransition::ATTACK, PlayerTransition::STOP });
}

void PlayerFSM::on_exit(PlayerState fromState)
{
    // do stuff
}

void PlayerFSM::on_enter(PlayerState toState)
{
    // do stuff
}