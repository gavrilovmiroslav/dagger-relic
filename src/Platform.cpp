#include "Platform.h"

void PlatformSystem::on_tick()
{
				
	const auto& keys = KeyState::get();
		
	for (auto&& [platform_entity, bindings, platform_pos, sprite] :access_storage().each())
	{	
		sprite.depth = -50;
		if (keys.is_down(bindings.down))
		{
			if(platform_pos.xy.y >  SCREEN_HEIGHT -64)
			{
				platform_pos.xy.y -= SPEED_MOD * Time::delta_time();
			}	
		}
	}		
}

