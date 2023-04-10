#include "PlayerControlsSystem.h"

void PlayerControlsSystem::on_tick() {
    for (auto&& [entity, player, pos, gravity, bindings] : access_storage().each())
		{
			const auto &keys = KeyState::get();
			if(keys.is_pressed(bindings.jump))
				player.speed={0, -15.5};
			player.speed+=gravity.acceleration;

			if(keys.is_down(bindings.left))
				pos.xy.x-=0.05f;

			if(keys.is_down(bindings.right))
				pos.xy.x+=0.05f;

			pos.xy += player.speed * SPEED_MOD * Time::delta_time();

			if(pos.xy.y+gravity.acceleration.y>=SCREEN_HEIGHT-BALL_RADIUS)
				pos.xy.y=SCREEN_HEIGHT-BALL_RADIUS;
			else
				pos.xy+=gravity.acceleration;	
		}
}