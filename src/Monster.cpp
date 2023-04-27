#include "Monster.h"
#include <iostream>

void MonsterSystem::on_tick()
{
	const auto &keys = KeyState::get();
	auto &ourGlobal = MutAccessUnique<OurGlobalVar>::access_unique();
	for (auto &&[player_entity, player, player_pos] : QueryPlayer::access_storage().each())
	{
		for (auto &&[monster_entity, monster, bindings, monster_pos, sprite] : QueryMonster::access_storage().each())
		{
			sprite.depth = -10;

			std::cout << monster_pos.xy.x << "     " << monster_pos.xy.y << std::endl;
			
			if (keys.is_down(bindings.up))
			{
				monster.verticalVelocity = 200;
			}
			if (!ourGlobal.isGrounded)
			{
				if (monster_pos.xy.y > SCREEN_HEIGHT - 320)
				{
					monster.verticalVelocity += GRAVITY * Time::delta_time();
					monster_pos.xy.y -= monster.verticalVelocity * Time::delta_time();
				}
			}
		}
	}
}
