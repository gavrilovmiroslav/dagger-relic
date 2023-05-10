#include "Components.h"

struct BoxMovingSystem
	: public ecs::System
	, public MutAccessGroupStorage<Box, Position, Gravity>
	, public MutAccessComponentById<Box>
{
	void on_tick() override;	
};