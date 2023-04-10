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
	geometry::Vec2 velocity;
	geometry::Vec2 velocitymax;
	geometry::Vec2 force;
	F32            moveforce;

	F32            mass;

	Movement(F32 moveforce, F32 velocitymax, F32 mass) : velocity(0.0f, 0.0f), moveforce(moveforce), velocitymax(velocitymax), force(0.0f, 0.0f), mass(mass) { }
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
};

struct CollisionSquareSystem
	: public ecs::System
	, public MutAccessGroupStorage<Position, Movement, CollisionSquare>
{
	void on_tick() override
	{
		for (auto&& [entity, pos, movement, collision] : access_storage().each())
		{
			//if (movement.velocity.x != 0.0f || movement.velocity.y != 0.0f)
			{
				for (auto&& [other_entity, other_pos, other_movement, other_collision] : access_storage().each())
				{
					if (entity <= other_entity)
					{
						continue;
					}

					if (pos.xy.x + collision.dimension > other_pos.xy.x &&
						pos.xy.x                       < other_pos.xy.x + other_collision.dimension &&
						pos.xy.y + collision.dimension > other_pos.xy.y &&
						pos.xy.y                       < other_pos.xy.y + other_collision.dimension)
					{
						/* This is really not accurate... Too bad! */
						F32 dvx = movement.velocity.x-other_movement.velocity.x;
						F32 dvy = movement.velocity.y-other_movement.velocity.y;

						other_movement.force.x = (movement.mass*dvx)/0.1f;
						other_movement.force.y = (movement.mass*dvy)/0.1f;
						movement.force.x       = (other_movement.mass*dvx)/0.1f;
						movement.force.y       = (other_movement.mass*dvy)/0.1f;
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
			F32 u = 6.2f;                               /* Friction coefficient. */
			F32 m = movement.mass;                      /* Object mass (kg). */
			F32 g = 9.807f;                             /* Gravity acceleration. */
			F32 n = m*g;                                /* Normal force. */
			F32 fnx = -fsignf(movement.velocity.x)*u*n; /* Friction force. */
			F32 fny = -fsignf(movement.velocity.y)*u*n; 
			F32 fx  = fnx+movement.force.x;             /* Total force. */
			F32 fy  = fny+movement.force.y;

			pos.xy              += movement.velocity*Time::delta_time();
			movement.velocity.x += (fx/m)*Time::delta_time();
			movement.velocity.y += (fy/m)*Time::delta_time();
			movement.velocity.x = fclampf(movement.velocity.x, -movement.velocitymax.x, movement.velocitymax.x);
			movement.velocity.y = fclampf(movement.velocity.y, -movement.velocitymax.y, movement.velocitymax.y);
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
				movement.force.x = -movement.moveforce;
				break;

				case DIR_Right:
				movement.force.x = movement.moveforce;
				break;

				case DIR_Up:
				movement.force.y = -movement.moveforce;
				break;

				case DIR_Down:
				movement.force.y = movement.moveforce;
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
			if (keys.is_down(keybinding.up))         movement.force.y = -movement.moveforce;
			else if (keys.is_down(keybinding.down))  movement.force.y =  movement.moveforce;
			else                                     movement.force.y -= fsignf(movement.force.y)*movement.moveforce;

			if (keys.is_down(keybinding.left))       movement.force.x = -movement.moveforce;
			else if (keys.is_down(keybinding.right)) movement.force.x =  movement.moveforce;
			else                                     movement.force.x -= fsignf(movement.force.x)*movement.moveforce;
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

		auto box2 = spawn()
				.with<KeyBindings>(KeyCode::KEY_LEFT, KeyCode::KEY_DOWN, KeyCode::KEY_UP, KeyCode::KEY_RIGHT)
				.with<Movement>(3000, 3000, 25.0f)
				.with<Sprite>(ecs::no_entity, 0)
				.with<SpriteAnimation>(Spritesheet::get_by_name("box/box_1"))
				.with<Visibility>(true)
				.with<Position>(geometry::Vec2{ 300, 300 })
				.with<CollisionSquare>(32)
				.done();

		auto box = spawn()
				.with<Box>()
				.with<Movement>(0, 3000, 10.0f)
				.with<Sprite>(ecs::no_entity, 0)
				.with<SpriteAnimation>(Spritesheet::get_by_name("box/box_1"))
				.with<Visibility>(true)
				.with<Position>(geometry::Vec2{ 300, 500 })
				.with<CollisionSquare>(32)
				.done();

		auto pushplate = spawn()
				.with<PushPlate>()
				.with<Movement>(0, 0, 1.0f)
				.with<Sprite>(ecs::no_entity, 0)
				.with<SpriteAnimation>(Spritesheet::get_by_name("pushplate/pushplate_1"))
				.with<Visibility>(true)
				.with<Position>(geometry::Vec2{ 100, 500 })
				.with<PostProcessText>("active: no", geometry::Vec2{100, 548})
				.done();

		auto boulder = spawn()
				.with<Boulder>()
				.with<Movement>(1000, 3000, 50.0f)
				.with<Sprite>(ecs::no_entity, 0)
				.with<SpriteAnimation>(Spritesheet::get_by_name("boulder/boulder_r1"))
				.with<Visibility>(true)
				.with<Position>(geometry::Vec2{ 500, 500 })
				.with<CollisionSquare>(48)
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
