#include "Background.h"

#include <iostream>

int counter = 0;
void BackgroundSystem::on_tick()
{
	const auto& keys = KeyState::get();
	auto& ourGlobal = MutAccessUnique<OurGlobalVar>::access_unique();
		
	for (auto&& [background_entity, bindings, background_pos, sprite] :access_storage().each())
	{
		sprite.depth = -100;
		
		if (keys.is_down(bindings.up))
		{
			background_pos.xy.y += SPEED_MOD * Time::delta_time();
		}	
		if (keys.is_down(bindings.down))
		{
			if(counter < 8)
			{
				background_pos.xy.y -= SPEED_MOD * Time::delta_time();
			}
			else
				ourGlobal.canPlayerMove = true;
			if(background_pos.xy.y <  -SCREEN_HEIGHT*2)
			{
				counter++;	
				despawn(background_entity);
			}
		}
	}
}
