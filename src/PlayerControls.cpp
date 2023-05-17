#include "PlayerControls.h"

void PlayerControlsSystem::on_tick()
{
	const auto &keys = KeyState::get();
	auto &ourGlobal = MutAccessUnique<OurGlobalVar>::access_unique();
	geometry::Vec2 tmp = {0,0};


	if (keys.is_pressed(KEY_ESCAPE))
	{
		Engine::get_instance().quit();
	}
	for (auto &&[entity, player, bindings, pos, sprite]: access_storage().each())
	{
		player_entity_cache = entity;
		if (!ourGlobal.isGrounded && ourGlobal.canPlayerMove)
		{
			player.verticalVelocity += GRAVITY * Time::delta_time();
			pos.xy.y += player.verticalVelocity * Time::delta_time();
		}

		if (sprite.is_finished())
		{
			fsm.trigger("animDone");
		}
		else
		{
			fsm.trigger("checkForHit");
		}

		if (ourGlobal.isGrounded && keys.is_down(bindings.attack))
		{
			fsm.trigger("attack");
		}
		else if (keys.is_down(bindings.left) || keys.is_down(bindings.right))
		{
			fsm.trigger("walk");
		}
		else
		{
			fsm.trigger("stop");
		}
	}
}
