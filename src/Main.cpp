#pragma once

#include "Prelude.h"

using namespace core;

struct Player {};

struct KeyBindings {
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
};

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SPEED_MOD 300.0f
#define GRAVITY_MOD 100.0f

struct GravitySystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, Position>
{
	void on_tick() override
	{
		for (auto&& [entity, pos] : access_storage().each())
		{
			if(pos.xy.y < SCREEN_HEIGHT - 16){
				pos.xy.y += GRAVITY_MOD * Time::delta_time();
				if(pos.xy.y > SCREEN_HEIGHT - 16){
					pos.xy.y = SCREEN_HEIGHT - 16;
				}
			}
		}
	}
};

struct PlayerControlsSystem 
	: public ecs::System
	, public MutAccessGroupStorage<Player, KeyBindings, Position, SpriteAnimation>
{
	void on_tick() override
	{
		const auto& keys = KeyState::get();

		for (auto&& [entity, bindings, pos, sprite] : access_storage().each())
		{
			if (keys.is_down(bindings.up))
			{
				pos.xy.y -= SPEED_MOD * Time::delta_time();
				if (pos.xy.y < 16) {
					pos.xy.y = 16;
				}
			}
			if (keys.is_down(bindings.down))
			{
				pos.xy.y += SPEED_MOD * Time::delta_time();
				if (pos.xy.y > SCREEN_HEIGHT - 16) {
					pos.xy.y = SCREEN_HEIGHT - 16;
				}
			}
			if (keys.is_down(bindings.left))
			{
				sprite.change_to("test/WizardRun");
				pos.xy.x -= SPEED_MOD * Time::delta_time();
				if (pos.xy.x < 11) {
					pos.xy.x = 11;
				}
			}
			if (keys.is_down(bindings.right))
			{
				sprite.change_to("test/WizardRun");
				pos.xy.x += SPEED_MOD * Time::delta_time();
				if (pos.xy.x > SCREEN_WIDTH - 11) {
					pos.xy.x = SCREEN_WIDTH - 11;
				}
			}
			else {
				sprite.change_to("test/WizardIdle");
			}
		}
	}
};

struct SWMG : public Game {
	SWMG()
	{
		auto& engine = Engine::get_instance();
		engine.use<PlayerControlsSystem>();
		engine.use<GravitySystem>();
	}

	void on_start() override{
		auto player_one = spawn()
			.with<Player>()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/WizardIdle"))
			.with<Position>(geometry::Vec2{ 50, 600 - 32 })
			.with<Visibility>(true)
			.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S, KeyCode::KEY_A, KeyCode::KEY_D)
			.done();

		auto player_two = spawn()
			.with<Player>()
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/WizardIdle"))
			.with<Position>(geometry::Vec2{ 750, 600 - 32 })
			.with<Visibility>(true)
			.with<KeyBindings>(KeyCode::KEY_UP, KeyCode::KEY_DOWN, KeyCode::KEY_LEFT, KeyCode::KEY_RIGHT)
			.done();
	}
};

int main(int argc, char* argv[])
{
	auto& engine = Engine::get_instance();
	engine.configure("dagger.ini", argc, argv);

	engine.setup<Default2D>();

	SWMG game;
	engine.run();

	return 0;
}

