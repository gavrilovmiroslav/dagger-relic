#include "PlayerControlsSystem.h"

void PlayerControlsSystem::on_tick() {
    for (auto&& [entity, player, pos, gravity, bindings, sprite, animation, flip] : access_storage().each())
		{
			const auto &keys = KeyState::get();
			if(keys.is_pressed(bindings.jump)){
				player.speed={0, -15.5};
				sprite.change_to(animation.jump);
			}
			player.speed+=gravity.acceleration;

			if(keys.is_down(bindings.left)){
				pos.xy.x-=0.05f;
				sprite.change_to(animation.walk);
				replace_component<Flip>(entity, Horizontal);
			}

			if(keys.is_down(bindings.right)){
				pos.xy.x+=0.05f;
				sprite.change_to(animation.walk);
				replace_component<Flip>(entity, None);
			}

			if(keys.is_released(bindings.left)){
				pos.xy.x-=0.05f;
				sprite.change_to(animation.idle);
			}

			if(keys.is_released(bindings.right)){
				pos.xy.x+=0.05f;
				sprite.change_to(animation.idle);
			}	

			pos.xy += player.speed * SPEED_MOD * Time::delta_time();

			if(pos.xy.y+gravity.acceleration.y>=SCREEN_HEIGHT-BALL_RADIUS){
				pos.xy.y=SCREEN_HEIGHT-BALL_RADIUS;
				player.on_platform=true;
			}
			else{
				pos.xy+=gravity.acceleration;
				player.on_platform=false;
			}	

			if(player.on_platform && !keys.is_down(bindings.left) && !keys.is_down(bindings.right))
				sprite.change_to(animation.idle);		
		}
}