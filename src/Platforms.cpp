#include "Platforms.h"


void PlatformSystem::on_tick()
{
	for (auto &&[player_entity, player, player_pos] : QueryPlayers::access_storage().each())
	{	
		bool on_platform=false;
		for (auto &&[platform_entity, platform, platform_pos] : QueryPlatforms::access_storage().each())
		{
			if (player_pos.xy.x - 60 <= platform_pos.xy.x + platform.width / 2  &&
			    player_pos.xy.x - 40 >= platform_pos.xy.x - platform.width / 2  &&
			    player_pos.xy.y - 18 >= platform_pos.xy.y - 1 &&
			    player_pos.xy.y - 18 <= platform_pos.xy.y + 1 &&
				player.speed.y>=0.0f
			    )
			{
				player.visina=player_pos.xy.y;
				player.speed.y=0.0f;
				on_platform=true;
				
				if(platform.id==1 && !is_spawned1){
					is_spawned1=true;
					auto platform6 = spawn()
					.with<Platform>(128.0f, 4.0f, 6)
					.with<Sprite>(ecs::no_entity)
					.with<SpriteAnimation>(Spritesheet::get_by_name("platform/platform"))
					.with<Position>(geometry::Vec2{250, 500})
					.with<Visibility>(true)
					.done();
				}
				if(platform.id==9 && player.id==1 && !rougue_on_platform){
					rougue_on_platform=true;
					auto platform7 = spawn()
					.with<Platform>(128.0f, 4.0f, 7)
					.with<Sprite>(ecs::no_entity)
					.with<SpriteAnimation>(Spritesheet::get_by_name("platform/platform"))
					.with<Position>(geometry::Vec2{550, 200})
					.with<Visibility>(true)
					.done();
				}
				if(platform.id==9 && player.id==2 && !knight_on_platform){
					knight_on_platform=true;
					auto platform8 = spawn()
					.with<Platform>(128.0f, 4.0f, 8)
					.with<Sprite>(ecs::no_entity)
					.with<SpriteAnimation>(Spritesheet::get_by_name("platform/platform"))
					.with<Position>(geometry::Vec2{250, 200})
					.with<Visibility>(true)
					.done();
				}
			
				break;
			}
		}
		player.isGrounded=on_platform;
	}

	for (auto &&[box_entity, box, box_pos] : QueryBoxes::access_storage().each())
	{

		bool on_platform=false;
		for (auto &&[platform_entity, platform, platform_pos] : QueryPlatforms::access_storage().each())
		{
			if (box_pos.xy.x - 13 <= platform_pos.xy.x + platform.width / 2  &&
			    box_pos.xy.x + 13 >= platform_pos.xy.x - platform.width / 2  &&
			    box_pos.xy.y + 30 >= platform_pos.xy.y - 1 &&
			    box_pos.xy.y + 30 <= platform_pos.xy.y + 1 &&
				box.speed.y>=0.0f
			    )
			{
				if(platform.id==2 && !box_on_flor){
					box_on_flor=true;
					auto platform9 = spawn()
					.with<Platform>(128.0f, 4.0f, 9)
					.with<Sprite>(ecs::no_entity)
					.with<SpriteAnimation>(Spritesheet::get_by_name("platform/platform"))
					.with<Position>(geometry::Vec2{400, 300})
					.with<Visibility>(true)
					.done();
				}
				box.speed.y=0.0f;
				on_platform=true;
				break;
			}
		}
		box.isGrounded=on_platform;
	}

	for (auto &&[box_entity, box, box_pos] : QueryBoxes::access_storage().each())
	{
		bool in_contact = false;
		for (auto &&[player_entity, player, player_pos] : QueryPlayers::access_storage().each())
		{
			if(((player_pos.xy.x - 40 >= box_pos.xy.x - 10 && player_pos.xy.x - 40 <= box_pos.xy.x - 5 && player.speed.x > 0.0f) ||
			   (player_pos.xy.x - 40 >= box_pos.xy.x + 27 && player_pos.xy.x - 40 <= box_pos.xy.x + 33 && player.speed.x < 0.0f)) &&
			    player_pos.xy.y - 47 >= box_pos.xy.y - 2 && player_pos.xy.y - 47 <= box_pos.xy.y + 32 &&
			    player.isGrounded)
			{
				box.speed.x = player.speed.x;
				in_contact = true;
			}
		}

		if(!in_contact)
			box.speed.x = 0.0f;
	}

}
