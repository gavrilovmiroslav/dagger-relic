#pragma once

#include "Prelude.h"
#include "Random.h"

#include "Boulder.h"
#include "Box.h"
#include "PushPlate.h"
#include "FloorComponent.h"

#define TEXTURE_SIZE 48

using namespace core;

struct Movement 
{
	geometry::Vec2 velocity;
	geometry::Vec2 velocitymax;
	geometry::Vec2 force;
	F32            moveforce;

	Movement(F32 moveforce, F32 velocitymax) : velocity(0.0f, 0.0f), moveforce(moveforce), velocitymax(velocitymax), force(0.0f, 0.0f) { }
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

			spdlog::info("force: {} {}", movement.force.x, movement.force.y);
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

		auto box = spawn()
				.with<Box>()
				.with<Movement>(0, 2000)
				.with<Sprite>(ecs::no_entity, 0)
				.with<SpriteAnimation>(Spritesheet::get_by_name("box/box_1"))
				.with<Visibility>(true)
				.with<Position>(geometry::Vec2{ 600, 500 })
				.done();

		auto pushplate = spawn()
				.with<PushPlate>()
				.with<Movement>(0, 2000)
				.with<Sprite>(ecs::no_entity, 0)
				.with<SpriteAnimation>(Spritesheet::get_by_name("pushplate/pushplate_1"))
				.with<Visibility>(true)
				.with<Position>(geometry::Vec2{ 100, 500 })
				.with<PostProcessText>("active: no", geometry::Vec2{0, 0})
				.done();

		auto boulder = spawn()
				.with<Boulder>()
				.with<Movement>(1000, 2000)
				.with<Sprite>(ecs::no_entity, 0)
				.with<SpriteAnimation>(Spritesheet::get_by_name("boulder/boulder_r1"))
				.with<Visibility>(true)
				.with<Position>(geometry::Vec2{ 300, 500 })
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
