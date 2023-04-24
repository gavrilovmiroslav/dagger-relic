#include "Components.h"

struct PlayerControlsSystem
	: public ecs::System
	, public MutAccessGroupStorage<Player, Position, Gravity, KeyBindings, SpriteAnimation, Animation, Flip>
	, public MutAccessComponentById<Player>
{
	void on_tick() override;	
};