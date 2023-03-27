#pragma once

#include "Prelude.h"

using namespace core;

struct Player {
	bool is_grounded;
	float jump_height;
};

struct KeyBindings {
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
};

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SPEED_MOD 300.0f
#define GRAVITY_MOD 300.0f
#define JUMP_SPEED_MOD 600.0f

struct GravitySystem // Should maybe be renamed to PhysicsSystem once more physics are implemented.
	: public ecs::System
	, public MutAccessGroupStorage<Player, Position>
{
	void on_tick() override
	{
		for (auto&& [player_entity, player, pos] : access_storage().each())
		{
			if(!player.is_grounded){
				pos.xy.y += GRAVITY_MOD * Time::delta_time();
				if(pos.xy.y > SCREEN_HEIGHT - 16){
					pos.xy.y = SCREEN_HEIGHT - 16;
					player.is_grounded = true;
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

		for (auto&& [player_entity, player, bindings, pos, sprite] : access_storage().each())
		{
			if (keys.is_down(bindings.up) && player.is_grounded)
			{
				player.is_grounded = false;
				player.jump_height = pos.xy.y - 100.0f;
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
			if (player.jump_height < pos.xy.y) {
				pos.xy.y -= JUMP_SPEED_MOD * Time::delta_time();
				if(pos.xy.y < player.jump_height){
					pos.xy.y = player.jump_height;
					player.jump_height = SCREEN_HEIGHT - 16;
				}
			}
			if (keys.is_released(bindings.up)){
				player.jump_height = SCREEN_HEIGHT - 16;
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
			.with<Player>(false, SCREEN_HEIGHT - 16)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/WizardIdle"))
			.with<Position>(geometry::Vec2{ 50, 600 - 32 })
			.with<Visibility>(true)
			.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S, KeyCode::KEY_A, KeyCode::KEY_D)
			.done();

		auto player_two = spawn()
			.with<Player>(false, SCREEN_HEIGHT - 16)
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

