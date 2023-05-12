#include "PlayerControlsSystem.h"

void PlayerControlsSystem::on_tick() {
    for (auto&& [entity, player, pos, gravity, bindings, sprite, animation, flip] : access_storage().each())
		{
			const auto &keys = KeyState::get();
			if(keys.is_pressed(bindings.jump) && player.isGrounded){
				player.speed={0, -12.5};
				sprite.change_to(animation.jump);
				player.isGrounded = false;
			}
			player.speed+=gravity.acceleration;

			if(keys.is_down(bindings.left)){
				player.speed.x = -0.70f;
				sprite.change_to(animation.walk);
				replace_component<Flip>(entity, Horizontal);
			}

			if(keys.is_down(bindings.right)){
				player.speed.x = 0.70f;
				sprite.change_to(animation.walk);
				replace_component<Flip>(entity, None);
			}

			if(keys.is_released(bindings.left)){
				player.speed.x = 0.0f;
				sprite.change_to(animation.idle);
			}

			if(keys.is_released(bindings.right)){
				player.speed.x = 0.0f;
				sprite.change_to(animation.idle);
			}

			
			pos.xy += player.speed * SPEED_MOD * Time::delta_time();
			
			

			if(player.isGrounded && !keys.is_down(bindings.left) && !keys.is_down(bindings.right))
				sprite.change_to(animation.idle);

			if(player.isGrounded)
				pos.xy.y=player.visina;

			if(pos.xy.y>618)
			{
				pos.xy.y=610;
				player.visina=610;
				player.isGrounded=true;
				player.speed.y=0.0f;
			}

			if(pos.xy.y <= 60.0f)
			{
				pos.xy.y = 60.0f;
				player.speed.y = 0.0f;
			}

			if(pos.xy.x <= 60.0f)
				pos.xy.x = 60.0f;

			if(pos.xy.x >= 840.0f)
				pos.xy.x = 840.0f;	

		}
}