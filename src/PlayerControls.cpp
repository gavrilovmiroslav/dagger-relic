#include "PlayerControls.h"

bool side = true;

void PlayerControlsSystem::on_tick()
{
	const auto& keys = KeyState::get();
	auto& ourGlobal = MutAccessUnique<OurGlobalVar>::access_unique();


	if (keys.is_pressed(KEY_ESCAPE))
	{
		Engine::get_instance().quit();
	}

	for (auto&& [entity, player, bindings, pos, sprite] :access_storage().each())
	{
		// if (keys.is_down(bindings.up))
		// {
			// 	background_pos.xy.y += SPEED_MOD * Time::delta_time();
		// }

		if (!ourGlobal.isGrounded && ourGlobal.canPlayerMove)
		{
			player.verticalVelocity += GRAVITY * Time::delta_time();
			pos.xy.y += player.verticalVelocity * Time::delta_time();
		}
		if (keys.is_down(bindings.left))
		{
			side = false;
			sprite.change_to("fallingHero/runningLeft");
			pos.xy.x -= SPEED_MOD * Time::delta_time();
			if(pos.xy.x < -35)
				pos.xy.x = -35;
		}
		else if (keys.is_down(bindings.right))
		{
			side = true;
			sprite.change_to("fallingHero/runningRight");
			pos.xy.x += SPEED_MOD * Time::delta_time();
			if(pos.xy.x > SCREEN_WIDTH - 70)
				pos.xy.x = SCREEN_WIDTH - 70;
		}
		else if (keys.is_down(bindings.attack))
		{
			if(side)
				sprite.change_to("fallingHero/attack");
			if(!side)
				sprite.change_to("fallingHero/attackLeft");
		}
		else
		{
			if(side)
				sprite.change_to("fallingHero/heroStanding");
			if(!side)
				sprite.change_to("fallingHero/leftHeroStanding");
		}
			
	}
}

