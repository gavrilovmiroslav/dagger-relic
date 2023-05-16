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
    currentState = MenuState::PAUSING;

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

/*void MenuFSM::load_game()
	{
		LevelManager level_manager;
		level_manager = LevelManager();
		level_manager.load_level("Levels/level1.txt");

		for(U32 i = 0; i < TILE_ROWS; i++)
		{
        		for(U32 j = 0; j < TILE_COLS; j++)
			{
				Char c = level_manager.level_map[j][i];
				if(c == 'x')
				{
					spawn()
					.with<Sprite>(ecs::no_entity, 6)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/wall"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96 + 25, j*96 + 25})
					.done();
				}
				if(c == 'o' || c == 'a' || c == 'b')
				{
					spawn()
					.with<Sprite>(ecs::no_entity, 2)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/sand"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96 +25, j*96+25})
					.done();
				}
				if(c == 'b')
				{
					spawn()
					.with<Sprite>(ecs::no_entity, 5)
					.with<SpriteAnimation>(Spritesheet::get_by_name("box/box_1"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96, j*96})
					.with<Movement>(2000.0f, 50.0f, 32, 32)
					.done();
				}
				if(c == 'p')
				{
					auto push_plate = spawn()
					.with<Sprite>(ecs::no_entity, 4)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pushplate/pushplate_3"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96, j*96})
					.with<KeyBinding>(KeyCode::KEY_LEFT, KeyCode::KEY_DOWN, KeyCode::KEY_UP, KeyCode::KEY_RIGHT, KeyCode::KEY_SPACE,KeyCode::KEY_A, KeyCode::KEY_P, KeyCode::KEY_ESCAPE)
					.done();

					//add_component<MenuClick>(push_plate, MenuClick{MenuFSM(push_plate)});
				}
				if(c == 'a')
				{
					auto player = spawn()
					.with<Sprite>(ecs::no_entity, 6)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/archaeologist_standing"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96, j*96})
					.with<Movement>(3000.0f, 50.0f, 16, 16)
					.with<KeyBinding>(KeyCode::KEY_LEFT, KeyCode::KEY_DOWN, KeyCode::KEY_UP, KeyCode::KEY_RIGHT, KeyCode::KEY_SPACE)
					.done();

					add_component<PlayerMovement>(player, PlayerMovement{PlayerFSM(player), 0.0f});
				}
        	}
		}
	}*/


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
	    //load_game();
            std::cout << "changed to PLAY" << std::endl;
            break;
        }
	case MenuState::EXITING:
	{
	   // sprite->change_to("exit1");
            std::cout << "changed to EXIT" << std::endl;
            break;
	}
        default:
            break;
    }

}
