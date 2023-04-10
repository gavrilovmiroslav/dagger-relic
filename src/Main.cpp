#pragma once

#include "Prelude.h"
#include "Random.h"

#include "Boulder.h"
#include "Box.h"
#include "PushPlate.h"
#include "FloorComponent.h"

#include <iostream>

#define TEXTURE_SIZE 48

using namespace core;

struct Movement 
{
	F32 velocity_x;
	F32 velocity_y;
};

struct KeyBindings 
{
	KeyCode left, down, up, right;
};

static float fsignf(float x)
{
	if (x > 0.0f) return  1.0f;
	if (x < 0.0f) return -1.0f;
	else          return  0.0f;
}

static float fclampf(float x, float min, float max)
{
	if (x < min) return min;
	if (x > max) return max;
	else         return x;
}

struct CollisionSquare
{
	I32 dimension;
	I32 offset_x;
	I32 offset_y;
	bool pushable;
};

struct CollisionSquareSystem
	: public ecs::System
	, public MutAccessGroupStorage<Position, Movement, CollisionSquare>
{
	void on_tick() override
	{
		for (auto&& [entity, pos, movement, collision] : access_storage().each())
		{
			if (movement.velocity_x != 0.0f || movement.velocity_y != 0.0f)
			{
				for (auto&& [other_entity, other_pos, other_movement, other_collision] : access_storage().each())
				{
					if (entity == other_entity)
					{
						continue;
					}

					if (other_collision.pushable == false &&
					    pos.xy.x + collision.dimension + collision.offset_x > other_pos.xy.x + other_collision.offset_x                             &&
						pos.xy.x + collision.offset_x                       < other_pos.xy.x + other_collision.offset_x + other_collision.dimension &&
						pos.xy.y + collision.dimension + collision.offset_y > other_pos.xy.y + other_collision.offset_y &&
						pos.xy.y + collision.offset_x                       < other_pos.xy.y + other_collision.offset_y + other_collision.dimension)
					{
						if (movement.velocity_x > 0.0f)
						{
							pos.xy.x = other_pos.xy.x;
							movement.velocity_x = 0.0f;
						}
						else if (movement.velocity_x < 0.0f)
						{
							pos.xy.x = other_pos.xy.x + other_collision.dimension;
							movement.velocity_x = 0.0f;
						}
						
						else if (movement.velocity_y > 0.0f)
						{
							pos.xy.y = other_pos.xy.y + other_collision.dimension;
							movement.velocity_y = 0.0f;
						}
						else if (movement.velocity_y < 0.0f)
						{
							pos.xy.y = other_pos.xy.y;
							movement.velocity_y = 0.0f;
						}
					}
				}
			}
		}
	}
};

struct MovementSystem
	: public ecs::System
	, public MutAccessGroupStorage<Movement, Position>
{
	void on_tick() override
	{
		for (auto&& [entity, movement, pos] : access_storage().each())
		{
			pos.xy.x += movement.velocity_x*Time::delta_time();
			pos.xy.y += movement.velocity_y*Time::delta_time();
		}
	}
};

struct DebugSquareSystem
	: public ecs::System
	, public MutAccessGroupStorage<Position, CollisionSquare, PostProcessSquare>
{
	void on_tick() override
	{
		for (auto&& [entity, pos, coll, square] : access_storage().each())
		{
			square.position.x = pos.xy.x + coll.offset_x;
			square.position.y = pos.xy.y + coll.offset_x;
		}
	}
};

struct BoulderControlSystem
	: public ecs::System
	, public MutAccessGroupStorage<Boulder, Movement>
{
	void on_tick() override
	{
		for (auto&& [entity, boulder, movement] : access_storage().each())
		{
			switch (boulder.direction)
			{
				case DIR_Left:
				movement.velocity_x = -64.0f;
				break;

				case DIR_Right:
				movement.velocity_x = 64.0f;
				break;

				case DIR_Up:
				movement.velocity_y = -64.0f;
				break;

				case DIR_Down:
				movement.velocity_y = 64.0f;
				break;
			}
		}
	}
};

struct MovementControlSystem
	: public ecs::System
	, public MutAccessGroupStorage<KeyBindings, Movement>
{
	void on_tick() override
	{
		const auto& keys = KeyState::get();

		for (auto&& [entity, keybinding, movement] : access_storage().each())
		{
			if (keys.is_down(keybinding.up))         movement.velocity_y = -50.0f;
			else if (keys.is_down(keybinding.down))  movement.velocity_y =  50.0f;
			else                                     movement.velocity_y =  0.0f;

			if (keys.is_down(keybinding.left))       movement.velocity_x = -50.0f;
			else if (keys.is_down(keybinding.right)) movement.velocity_x =  50.0f;
			else                                     movement.velocity_x =  0.0f;
		}
	}
};

struct Pong : public Game
{
	DynamicArray<DynamicArray<FloorType>> grid;

	Pong()
	{
		auto& engine = Engine::get_instance();
		engine.use<MovementSystem>();
		engine.use<MovementControlSystem>();
		engine.use<BoulderControlSystem>();
		engine.use<CollisionSquareSystem>();
		engine.use<DebugSquareSystem>();

		grid.resize(800/TEXTURE_SIZE+1);
		for(int i=0;i<grid.size();i++)
		{
			grid[i].resize(600/TEXTURE_SIZE+1);
			for(int j=0;j<grid[i].size();j++)
			{
				grid[i][j]=FT_Sand;
			}
		}
	}

	void on_start() override
	{
		for(int i=0;i<grid.size();i++)
		{
			for(int j=0;j<grid[i].size();j++)
			{
				auto floor = spawn()
						.with<Floor>(grid[i][j])
						.with<Sprite>(ecs::no_entity, -999)
						.with<SpriteAnimation>(Spritesheet::get_by_name("pyramidplunder/sand"))
						.with<Visibility>(true)
						.with<Position>(geometry::Vec2{TEXTURE_SIZE*i+25,TEXTURE_SIZE*j+25}) //idk, the screen starts at (25,25)
						.done();
			}
		}

		auto player = spawn()
				.with<KeyBindings>(KeyCode::KEY_LEFT, KeyCode::KEY_DOWN, KeyCode::KEY_UP, KeyCode::KEY_RIGHT)
				.with<Movement>()
				.with<Sprite>(ecs::no_entity, 0)
				.with<SpriteAnimation>(Spritesheet::get_by_name("box/box_1"))
				.with<Visibility>(true)
				.with<Position>(geometry::Vec2{ 300, 300 })
				.with<CollisionSquare>(32, 8, 8, false)
				.with<PostProcessSquare>(32u, geometry::Vec2{ 0, 0 })
				.done();

		auto boxwall = spawn()
				.with<Box>()
				.with<Movement>()
				.with<Sprite>(ecs::no_entity, 0)
				.with<SpriteAnimation>(Spritesheet::get_by_name("box/box_1"))
				.with<Visibility>(true)
				.with<Position>(geometry::Vec2{ 300, 500 })
				.with<CollisionSquare>(32, 8, 8, false)
				.with<PostProcessSquare>(32u, geometry::Vec2{ 0, 0 })
				.done();
		auto boxwalla2 = spawn()
				.with<Box>()
				.with<Movement>()
				.with<Sprite>(ecs::no_entity, 0)
				.with<SpriteAnimation>(Spritesheet::get_by_name("box/box_1"))
				.with<Visibility>(true)
				.with<Position>(geometry::Vec2{ 300, 400 })
				.with<CollisionSquare>(32, 8, 8, false)
				.with<PostProcessSquare>(32u, geometry::Vec2{ 0, 0 })
				.done();

		auto pushplate = spawn()
				.with<PushPlate>()
				.with<Movement>()
				.with<Sprite>(ecs::no_entity, 0)
				.with<SpriteAnimation>(Spritesheet::get_by_name("pushplate/pushplate_1"))
				.with<Visibility>(true)
				.with<Position>(geometry::Vec2{ 100, 500 })
				.with<PostProcessText>("active: no", geometry::Vec2{100, 548})
				.done();

		auto boulder = spawn()
				.with<Boulder>()
				.with<Movement>()
				.with<Sprite>(ecs::no_entity, 0)
				.with<SpriteAnimation>(Spritesheet::get_by_name("boulder/boulder_r1"))
				.with<Visibility>(true)
				.with<Position>(geometry::Vec2{ 500, 500 })
				//.with<CollisionSquare>(48, false)
				.done();
	}
};

int main(int argc, char* argv[])
{
	auto& engine = Engine::get_instance();
	std::string filePath = "dagger.ini";
	engine.configure(filePath.data(), argc, argv);

	engine.setup<Default2D>();
	engine.setup<PostProcess2D>();

	Pong game;
	engine.run();

	return 0;
}
