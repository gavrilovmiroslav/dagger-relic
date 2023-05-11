#include "Platforms.h"
#include <iostream>

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
