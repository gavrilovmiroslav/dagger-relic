#pragma once

#include "Prelude.h"
#include "GameComponents.h"
#include "GameSystem.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
using namespace core;


struct FallingHero 
	: public Game
	, public AllocateUnique<OurGlobalVar>
{
	FallingHero()
	{
		auto& engine = Engine::get_instance();
		engine.use<PlayerControlsSystem>();
		engine.use<BackgroundSystem>();
		engine.use<PlatformSystem>();
		engine.use<ItemSystem>();
	}

	void on_start() override
	{	
		std::ifstream inputFile();
		std::ifstream infile("data/levelOne/coinPosition.txt");
		int rows, cols;
		infile >> rows >> cols;
		std::vector<std::vector<char>> matrix(rows, std::vector<char>(cols));
		for (int i = 0; i < rows; ++i)
		{
			for (int j = 0; j < cols; ++j)
			{
				infile >> matrix[i][j];
			}
		}

		infile.close();

		for(int i = 0; i<NUMBER_OF_PLATFORM; i++)
			spawn()
				.with<Platform>(0.0f)
				.with<Sprite>(ecs::no_entity)
				.with<SpriteAnimation>(Spritesheet::get_by_name("platform/platformPack"))
				.with<Position>(geometry::Vec2{ 64*i, SCREEN_HEIGHT*NUMBER_OF_BACKGROUND-64})
				.with<Visibility>(true)
				.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S).done();
		for(int i = 0; i<NUMBER_OF_BACKGROUND; i++)
			spawn()
				.with<Background>(0.0f)
				.with<Sprite>(ecs::no_entity)
				.with<SpriteAnimation>(Spritesheet::get_by_name("background/bg47"))
				.with<Scale>(geometry::Vec2{4,3})
				.with<Position>(geometry::Vec2{ 0, SCREEN_HEIGHT*i })
				.with<Visibility>(true)
				.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S).done();
		auto hero = spawn()
			.with<Player>(0.0f, 0)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("fallingHero/heroStanding"))
			.with<Position>(geometry::Vec2{ 300, 100 })
			.with<Scale>(geometry::Vec2{3,3})
			.with<Visibility>(true)
			.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S, KeyCode::KEY_A, KeyCode::KEY_D, KeyCode::KEY_SPACE)
			.done();
		
		for (int i = 0; i < rows; ++i)
		{
			for (int j = 0; j < cols; ++j)
			{
				if(matrix[i][j] == '*')
				{
					auto coin = spawn()
						.with<Item>(0.0f, 100.0f)
						.with<Sprite>(ecs::no_entity)
						.with<SpriteAnimation>(Spritesheet::get_by_name("coin/goldCoin"))
						.with<Position>(geometry::Vec2{j*SCREEN_WIDTH/cols ,i*SCREEN_HEIGHT/4})
						.with<Scale>(geometry::Vec2{3,3})
						.with<Visibility>(true)
						.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S).done();
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


	FallingHero game;
	engine.run();

	return 0;
}

