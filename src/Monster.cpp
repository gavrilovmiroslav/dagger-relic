#include "Monster.h"
#include <random>
#include <iostream>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);
double randomNumber = dis(gen);
auto randomIndicator = 0;
void MonsterSystem::on_tick()
{
	const auto &keys = KeyState::get();
	auto &ourGlobal = MutAccessUnique<OurGlobalVar>::access_unique();
	if(randomIndicator>100)
	{
		randomNumber = dis(gen);
		randomIndicator = 0;
	}
	randomIndicator++;
	for (auto &&[player_entity, player, player_pos] : QueryPlayer::access_storage().each())
	{
		for (auto &&[monster_entity, monster, bindings, monster_pos, sprite, spriteAnim] : QueryMonster::access_storage().each())
		{
			monster_entity_cache = monster_entity;
			player_entity_cache = player_entity;
			sprite.depth = -10;
			if (keys.is_down(bindings.up))
			{
				monster.verticalVelocity = 200;
			}
			if (!ourGlobal.isGrounded)
			{
				if (monster_pos.xy.y > SCREEN_HEIGHT - 280)
				{
					monster.verticalVelocity += GRAVITY * Time::delta_time();
					monster_pos.xy.y -= monster.verticalVelocity * Time::delta_time();
				}
			}
			if (spriteAnim.is_finished())
			{
				fsm.trigger("animDone");
			}
			else
			{
				fsm.trigger("checkForHit");
			}
			if (ourGlobal.isGrounded && abs(player_pos.xy.x-monster_pos.xy.x)<50 && randomNumber<0.2)
			{
				fsm.trigger("attack");
			}
			else if(ourGlobal.isGrounded)
			{
				fsm.trigger("walk");
			}
			else
			{
				fsm.trigger("stop");
			}
		}
	}
}
