#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include "include/window.h"
#include "MainMenu.h"
#include "EngineButton.h"

using namespace std;


/*int main(int argc, char *argv[]){

    bool quit = false;
    bool showNewWindow = false;

    SDL_Event event;

    EngineWindow appWindow;
    EngineWindow newWindow;
    appWindow.createWindow("Pyramid plunder", 800, 600);

    EngineMenu engineMenu(appWindow.renderer, appWindow.mainWindow);

    //init the splash screen
    engineMenu.initSplashScreen("Press Enter to start", "liberation.ttf", "data/pyramids2.bmp");

    EngineButton playButton(appWindow.renderer, "data/play1.bmp", 325, 200, 150, 40);
    playButton.setCallback([&](){
        showNewWindow = true;
    });
    EngineButton optionsButton(appWindow.renderer, "data/options1.bmp", 325, 300, 150, 40);
    optionsButton.setCallback([&](){
        showNewWindow = true;
    });
    EngineButton exitButton(appWindow.renderer, "data/exit1.bmp", 325, 400, 150, 40);
    exitButton.setCallback([&](){
        showNewWindow = true;
    });


    while (!quit){

        SDL_PollEvent(&event);
        if(event.type==SDL_WINDOWEVENT && event.window.event==SDL_WINDOWEVENT_CLOSE){
            quit = true;
        }

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        playButton.isClicked(mouseX, mouseY);

        SDL_RenderClear(appWindow.renderer);

        engineMenu.displaySplashScreen();
        playButton.display();
        optionsButton.display();
        exitButton.display();
        

        //clear the render
        //SDL_RenderClear(appWindow.renderer);

        //render the splash screen
        //engineMenu.displaySplashScreen();

        //render the new texture
        SDL_RenderPresent(appWindow.renderer);
        SDL_Delay(20);

        if (showNewWindow) {
            newWindow.createWindow("Pyramid plunder", 800, 600);
            showNewWindow = false;
        }

        // Destroy the new window when the event loop ends
        showNewWindow = false;
    }
    

    playButton.setCallback(nullptr);
    optionsButton.setCallback(nullptr);
    exitButton.setCallback(nullptr);
    
    engineMenu.quitSplashScreen();

    appWindow.destroyWindow();
    newWindow.destroyWindow();

    return 0;

}*/

#pragma once

#include "Algebra.h"
#include "LevelManager.h"
#include "Player.h"
#include "Prelude.h"
#include "Random.h"

using namespace core;

struct Movement
{
	geometry::Vec2 velocity;
	geometry::Vec2 velocity_max;
	geometry::Vec2 force;
	F32            move_force;
	F32            collision_box_width;
	F32            collision_box_height;

	Movement(F32 move_force, F32 velocity_max, F32 collision_w, F32 collision_h)
		: velocity(0.0f, 0.0f)
		, move_force(move_force)
		, velocity_max(velocity_max)
		, force(0.0f, 0.0f)
		, collision_box_width(collision_w)
		, collision_box_height(collision_h)
	{

	}
};

struct KeyBinding
{
	KeyCode left, down, up, right;
	KeyCode blindfold_change;
};

struct MovementSystem
	: public ecs::System
	, public MutAccessGroupStorage<Movement, Position>
{
	void on_tick() override
	{
		for (auto&& [entity, movement, pos] : access_storage().each())
		{
			F32 u = 16.0f;                              /* Friction coefficient. */
			F32 m = 5.0f;                               /* Object mass (kg). */
			F32 g = 9.807f;                             /* Gravity acceleration. */
			F32 n = m*g;                                /* Normal force. */
			F32 fnx = -fsignf(movement.velocity.x)*u*n; /* Friction force. */
			F32 fny = -fsignf(movement.velocity.y)*u*n;
			F32 fx  = fnx+movement.force.x;             /* Total force. */
			F32 fy  = fny+movement.force.y;

			for (auto&& [entity2, movement2, pos2] : access_storage().each())
			{
				F32 dx, dy;
				F32 collision_width = movement.collision_box_width + movement2.collision_box_width;
				F32 collision_height = movement.collision_box_height + movement2.collision_box_height;

				if (entity <= entity2)
				{
					continue;
				}

				dx = pos2.xy.x - pos.xy.x;
				dy = pos2.xy.y - pos.xy.y;

				if (fabsf(dx) < collision_width / 2.0f && fabsf(dy) < collision_height / 2.0f)
				{
					if (fabsf(dx) > fabsf(dy))
					{
						if (dx > 0.0f)
						{
							pos.xy.x -= collision_width / 2.0f - dx;
						}
						else
						{
							pos.xy.x += collision_width / 2.0f + dx;
						}

						/*
						movement.velocity.x = 0.0f;
						movement2.velocity.x = 0.0f;
						*/
					}
					else
					{
						if (dy > 0.0f)
						{
							pos.xy.y -= collision_height / 2.0f - dy;
						}
						else
						{
							pos.xy.y += collision_height / 2.0f + dy;
						}

						/*
						movement.velocity.y = 0.0f;
						movement2.velocity.y = 0.0f;
						*/
					}
				}

			}

			pos.xy              += movement.velocity*Time::delta_time();
			movement.velocity.x += (fx/m)*Time::delta_time();
			movement.velocity.y += (fy/m)*Time::delta_time();
			movement.velocity.x = fclampf(movement.velocity.x, -movement.velocity_max.x, movement.velocity_max.x);
			movement.velocity.y = fclampf(movement.velocity.y, -movement.velocity_max.y, movement.velocity_max.y);
		}
	}
};

struct ClickControlSystem
	: public ecs::System
{
	void on_tick() override
	{
		const auto& key = KeyState::get();

		if (key.is_down(KeyCode::KEY_ESCAPE))
		{
			Engine::get_instance().quit();
		}

	}
};

struct MovementControlSystem
	: public ecs::System
	, public MutAccessGroupStorage<KeyBinding, Movement, SpriteAnimation>
{
	void on_tick() override
	{
		const auto& key = KeyState::get();

		for (auto&& [entity, key_binding, movement, sprite] : access_storage().each())
		{
			if (key.is_down(key_binding.up))
			{
				movement.force.y = -movement.move_force;
			}
			else if (key.is_down(key_binding.down))
			{
				movement.force.y =  movement.move_force;
			}
			else
			{
				movement.force.y -= fsignf(movement.force.y)*movement.move_force;
			}

			if (key.is_down(key_binding.left))
			{
				replace_component<Flip>(entity, Horizontal);
				sprite.change_to("pyramidplunder/archaeologist_running");
				movement.force.x = -movement.move_force;
			}
			else if (key.is_down(key_binding.right))
			{
				replace_component<Flip>(entity, None);
				sprite.change_to("pyramidplunder/archaeologist_running");
				movement.force.x =  movement.move_force;
			}
			else
			{
				movement.force.x -= fsignf(movement.force.x)*movement.move_force;
			}

			spdlog::info("force: {} {}", movement.force.x, movement.force.y);
		}
	}
};

// TODO: move to separate file after refactoring Main.cpp and add signals to other components
struct BlindfoldChangingSystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, KeyBinding>
{
	void on_tick() override
	{
		const auto& keys = KeyState::get();

		U32 counter = 0;
		StaticArray<SpecialBlindfold, 3> blindfolds = { SpecialBlindfold::HumanEyes, SpecialBlindfold::FoxEyes, SpecialBlindfold::ScorpionEyes };

		for (auto&& [entity, player, key_binding] : access_storage().each())
		{
			SpecialBlindfold new_blindfold;
			if (keys.is_pressed(key_binding.blindfold_change))
			{
				counter++;
				new_blindfold = blindfolds[counter % 3];

				if (player.available_blindfolds[new_blindfold] != 0)
				{
					player.current_blindfold = new_blindfold;
				}
			}
		}
	}
};

struct PyramidPlunder : public Game
{
	LevelManager level_manager;

	PyramidPlunder()
	{
		auto& engine = Engine::get_instance();
		engine.use<BlindfoldChangingSystem>();
		engine.use<MovementSystem>();
		engine.use<MovementControlSystem>();
		engine.use<ClickControlSystem>();
	}

	void on_start() override
	{
       
		level_manager = LevelManager();
		level_manager.load_level("Levels/level2.txt");

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
					spawn()
					.with<Sprite>(ecs::no_entity, 4)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pushplate/pushplate_3"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96, j*96})
					.done();
				}
				if(c == 'a')
				{
					spawn()
					.with<Player>(SpecialBlindfold::HumanEyes)
					.with<Sprite>(ecs::no_entity, 6)
					.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/archaeologist_standing"))
					.with<Visibility>(true)
					.with<Position>(geometry::Vec2{ i*96, j*96})
					.with<Movement>(3000.0f, 50.0f, 16, 16)
					.with<KeyBinding>(KeyCode::KEY_LEFT, KeyCode::KEY_DOWN, KeyCode::KEY_UP, KeyCode::KEY_RIGHT, KeyCode::KEY_SPACE)
					.done();
				}
        	}
		}
    }

};

int main(int argc, char* argv[])
{
	auto& engine = Engine::get_instance();
	std::string filePath = "dagger.ini";
	engine.configure(filePath.data(), argc, argv);

	engine.setup<Default2D>();

	PyramidPlunder game;
	engine.run();

	return 0;
}