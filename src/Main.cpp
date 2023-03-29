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

struct Pickup {
	const char* name = new char(51);
	double radius;
	bool is_picked;
};

struct BasicAttack {
	float speed;
	float range;
	float radius;
};

struct KeyBindings {
	KeyCode up;
	KeyCode down;
	KeyCode left;
	KeyCode right;
	KeyCode basic_attack;
};

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SPEED_MOD 300.0f
#define GRAVITY_MOD 200.0f
#define JUMP_MOD 600.0f

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
	, public MutAccessGroupStorage<Player, KeyBindings, Position, SpriteAnimation, Flip>
{
	void on_tick() override
	{
		const auto& keys = KeyState::get();

		for (auto&& [player_entity, player, bindings, pos, sprite, flip] : access_storage().each())
		{
			if (keys.is_pressed(bindings.up) && player.is_grounded) // keys.is_down() causes a bug where player can levitate trough platforms 
			{
				player.is_grounded = false;
				player.jump_speed = JUMP_MOD;
			}
			if (keys.is_down(bindings.down))  // deleted player.is_grounded condition, player can now go down from platform
			{
				pos.xy.y += SPEED_MOD * Time::delta_time();
				if (pos.xy.y > SCREEN_HEIGHT - 16) {
					pos.xy.y = SCREEN_HEIGHT - 16;
				}
			}
			if (keys.is_down(bindings.left))
			{
				replace_component<Flip>(player_entity, Horizontal);
				sprite.change_to("test/WizardRun");
				pos.xy.x -= SPEED_MOD * Time::delta_time();
				if (pos.xy.x < 11) {
					pos.xy.x = 11;
				}
			}
			if (keys.is_down(bindings.right))
			{
				replace_component<Flip>(player_entity, None);
				sprite.change_to("test/WizardRun");
				pos.xy.x += SPEED_MOD * Time::delta_time();
				if (pos.xy.x > SCREEN_WIDTH - 11) {
					pos.xy.x = SCREEN_WIDTH - 11;
				}
			}
			else if (!keys.is_down(bindings.left)){
				sprite.change_to("test/WizardIdle");
			}
			if (player.jump_speed > 0) {
				pos.xy.y -= player.jump_speed * Time::delta_time();
				player.jump_speed -= 0.1;
			}
			if (keys.is_released(bindings.up)){
				player.jump_speed/=2;
			}
			if (keys.is_pressed(bindings.basic_attack)){
				if (flip == None){
					auto attack = spawn()
					.with<BasicAttack>(300.0f, pos.xy.x + 400.0f, 25.0f)
					.with<Sprite>(ecs::no_entity)
					.with<SpriteAnimation>(Spritesheet::get_by_name("test/Fireball"))
					.with<Position>(geometry::Vec2{ pos.xy.x + 30.0f , pos.xy.y })
					.with<Visibility>(true)
					.with<Flip>(None)
					.with<Scale>(geometry::Vec2{0.1f,0.1f}); // There must exist a better way to scale objects.
				}
				else{
					auto attack = spawn()
					.with<BasicAttack>(300.0f, pos.xy.x - 400.0f, 25.0f)
					.with<Sprite>(ecs::no_entity)
					.with<SpriteAnimation>(Spritesheet::get_by_name("test/Fireball"))
					.with<Position>(geometry::Vec2{ pos.xy.x - 30.0f , pos.xy.y })
					.with<Visibility>(true)
					.with<Flip>(Horizontal)
					.with<Scale>(geometry::Vec2{0.1f,0.1f}); // There must exist a better way to scale objects.
				}
			}
		}
	}
};

struct PickupSystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, Position>
	, public MutAccessGroupStorage<Pickup, Position>
{
	using QueryPlayers 	= MutAccessGroupStorage<Player, Position>;
	using QueryPlickups = MutAccessGroupStorage<Pickup, Position>;
	
	void on_tick() override
	{
		for (auto&& [player_entity, player, player_pos] : QueryPlayers :: access_storage().each())
		{
			for (auto&& [pickup_entity, pickup, pickup_pos] : QueryPlickups :: access_storage().each())
			{
				if (	abs(player_pos.xy.x - pickup_pos.xy.x) <= pickup.radius
					&& 	abs(player_pos.xy.y - pickup_pos.xy.y) <= pickup.radius
					&&  !pickup.is_picked){
						pickup.is_picked = true;

						replace_component<Pickup>(player_entity, pickup.name,pickup.radius, pickup.is_picked);
						despawn(pickup_entity);
				}
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
				if(	player_pos.xy.x <= platform_pos.xy.x + platform.width / 2 + 11
					&& player_pos.xy.x >= platform_pos.xy.x - platform.width / 2 - 11
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

struct CombatSystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, Position>
	, public MutAccessGroupStorage<BasicAttack, Position, SpriteAnimation, Flip>
{
	using QueryPlayers = MutAccessGroupStorage<Player, Position>;
	using QueryAttacks = MutAccessGroupStorage<BasicAttack, Position, SpriteAnimation, Flip>;

	void on_tick() override
	{
		for(auto&& [attack_entity, attack, attack_pos, sprite, flip] : QueryAttacks :: access_storage().each()){
			if(flip == None){
				attack_pos.xy.x += attack.speed * Time::delta_time();
				if(attack_pos.xy.x > attack.range){
					despawn(attack_entity);
				}
			}
			else{
				attack_pos.xy.x -= attack.speed * Time:: delta_time();
				if(attack_pos.xy.x < attack.range){
					despawn(attack_entity);
				}
			}

			for(auto&& [player_entity, player, player_pos] : QueryPlayers :: access_storage().each()){
				if (	abs(player_pos.xy.x - attack_pos.xy.x) <= attack.radius
					&& 	abs(player_pos.xy.y - attack_pos.xy.y) <= attack.radius){
						despawn(player_entity); // Just completely despawns the guy, for now.
						despawn(attack_entity);
					}
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
		engine.use<PlatformSystem>();
		engine.use<PickupSystem>();
		engine.use<CombatSystem>();
	}

	void on_start() override{

		auto pickup1 = spawn()
			.with<Pickup>("potion" ,32, false)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/pickup"))
			.with<Position>(geometry::Vec2{ 600, 570 })
			.with<Visibility>(true);

		auto pickup2 = spawn()
			.with<Pickup>("scroll" ,32, false)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/pickup"))
			.with<Position>(geometry::Vec2{ 200, 400 })
			.with<Visibility>(true);
	
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

		auto player_one = spawn()
			.with<Player>(false, 0)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/WizardIdle"))
			.with<Position>(geometry::Vec2{ 50, 600 - 32})
			.with<Visibility>(true)
			.with<KeyBindings>(KeyCode::KEY_W, KeyCode::KEY_S, KeyCode::KEY_A, KeyCode::KEY_D, KeyCode::KEY_F)
			.with<Flip>(None)
			.done();

		auto player_two = spawn()
			.with<Player>(false, 0)
			.with<Sprite>(ecs::no_entity)
			.with<SpriteAnimation>(Spritesheet::get_by_name("test/WizardIdle"))
			.with<Position>(geometry::Vec2{ 750, 600 - 32})
			.with<Visibility>(true)
			.with<KeyBindings>(KeyCode::KEY_UP, KeyCode::KEY_DOWN, KeyCode::KEY_LEFT, KeyCode::KEY_RIGHT, KeyCode::KEY_APOSTROPHE)
			.with<Flip>(Horizontal)
			.done();
	}
};

int main(int argc, char* argv[])
{
	auto& engine = Engine::get_instance();
	std::string filePath = "dagger.ini";
	engine.configure(filePath.data(), argc, argv);

	engine.setup<Default2D>();

	SWMG game;
	engine.run();

	return 0;
}

