#pragma once

#include "Prelude.h"

using namespace core;

struct Player {
	bool is_grounded;
	float jump_speed;
};

struct Platform {
	float width;
	float height;
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
#define GRAVITY_MOD 200.0f
#define JUMP_MOD 500.0f

struct GravitySystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, Position>
{
	void on_tick() override
	{
		for (auto&& [player_entity, player, pos] : access_storage().each())
		{
			if(!player.is_grounded){
				pos.xy.y += GRAVITY_MOD * Time::delta_time();
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
			if (keys.is_pressed(bindings.up) && player.is_grounded) // keys.is_down() causes a bug where player can levitate trough platforms 
			{
				player.is_grounded = false;
				player.jump_speed = JUMP_MOD;
			}
			if (keys.is_down(bindings.down) && !player.is_grounded) 
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
			if (player.jump_speed > 0) {
				pos.xy.y -= player.jump_speed * Time::delta_time();
				player.jump_speed -= 0.1;
			}
			if (keys.is_released(bindings.up)){
				player.jump_speed/=2;
			}
		}
	}
};

struct PlatformSystem 
	: public ecs::System
	, public MutAccessGroupStorage<Player, Position>
	, public AccessGroupStorage<Platform, Position>
{
	using QueryPlayers = MutAccessGroupStorage<Player, Position>;
	using QueryPlatforms = AccessGroupStorage<Platform, Position>;
	
	void on_tick() override
	{
		for (auto&& [player_entity, player, player_pos] : QueryPlayers :: access_storage().each())
		{
			bool on_platfrom = false;
			for (auto&& [platform_entity, platform, platform_pos] : QueryPlatforms :: access_storage().each())
			{
				if(	player_pos.xy.x <= platform_pos.xy.x + platform.width / 2 
					&& player_pos.xy.x >= platform_pos.xy.x - platform.width / 2
					&& player_pos.xy.y + 16 >= platform_pos.xy.y - 1
					&& player_pos.xy.y + 16 <= platform_pos.xy.y + 1) 
					{
						on_platfrom = true;
						break;
					}
			}
			player.is_grounded = on_platfrom;	
		}
	}
};

struct SWMG : public Game {
	SWMG()
	{
		auto& engine = Engine::get_instance();
		engine.use<PlayerControlsSystem>();
		engine.use<GravitySystem>();
		engine.use<PlatformSystem>();
	}

	void on_start() override{
		auto player_one = spawn()
			.with<Player>(false, 0)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/WizardIdle"))
			.with<Position>(geometry::Vec2{ 50, 600 - 32})
			.with<Visibility>(true)
			.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S, KeyCode::KEY_A, KeyCode::KEY_D)
			.done();

		auto player_two = spawn()
			.with<Player>(false, 0)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/WizardIdle"))
			.with<Position>(geometry::Vec2{ 750, 600 - 32})
			.with<Visibility>(true)
			.with<KeyBindings>(KeyCode::KEY_UP, KeyCode::KEY_DOWN, KeyCode::KEY_LEFT, KeyCode::KEY_RIGHT)
			.done();

		auto platform1 = spawn()
			.with<Platform>(128, 4)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/platform"))
			.with<Position>(geometry::Vec2{ 400, 550 })
			.with<Visibility>(true);

		auto platform2 = spawn()
			.with<Platform>(128, 4)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/platform"))
			.with<Position>(geometry::Vec2{ 200, 450 })
			.with<Visibility>(true);

		auto platform3 = spawn()
			.with<Platform>(128, 4)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/platform"))
			.with<Position>(geometry::Vec2{ 300, 500 })
			.with<Visibility>(true);

		auto platform4 = spawn()
			.with<Platform>(128, 4)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/platform"))
			.with<Position>(geometry::Vec2{ 450, 450 })
			.with<Visibility>(true);

		auto platform5 = spawn()
			.with<Platform>(128, 4)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/platform"))
			.with<Position>(geometry::Vec2{ 600, 400 })
			.with<Visibility>(true);		

		auto ground = spawn()
			.with<Platform>(SCREEN_WIDTH, 4)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/platform"))
			.with<Position>(geometry::Vec2{ SCREEN_WIDTH / 2, SCREEN_HEIGHT })
			.with<Visibility>(true);

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

