#include "MenuFSM.h"

#include <iostream>

std::string stateName(MenuState state) {
    switch(state) {
        case PAUSING: return "PAUSING"; break;
        case EXITING: return "EXITING"; break;
        case PLAYING: return "PLAYING"; break;
    }
}

MenuFSM::MenuFSM(entt::entity entity)
    : id(entity)
{
    currentState = MenuState::PLAYING;

    add_transition_to_new_state(MenuTransition::PAUSE, MenuState::PAUSING);
    add_transition_to_new_state(MenuTransition::EXIT, MenuState::EXITING);
    add_transition_to_new_state(MenuTransition::PLAY, MenuState::PLAYING);

    add_valid_transitions_from_state( MenuState::PLAYING,
        containers::Set<MenuTransition> { MenuTransition::PAUSE });
    add_valid_transitions_from_state( MenuState::PAUSING,
        containers::Set<MenuTransition> { MenuTransition::PLAY, MenuTransition::EXIT });
}

void MenuFSM::on_exit(MenuState fromState)
{
    std::cout << "exiting state: [" << stateName(fromState) << "], ";
}

void MenuFSM::on_enter(MenuState toState)
{
    std::cout << "entering state: [" << stateName(toState) << "]" << std::endl;

    switch (toState)
    {
        case MenuState::PAUSING:
        {
            std::cout << "changed to PAUSE" << std::endl;
            break;
        }
        case MenuState::PLAYING:
        {
            std::cout << "changed to PLAY" << std::endl;
            break;
        }
	case MenuState::EXITING:
	{
            std::cout << "changed to EXIT" << std::endl;
            break;
	}
        default:
            break;
    }

}
