#pragma once

#include "Definitions.h"
#include "Access.h"
#include "GameSpecificComponents.h"
#include "Geometry.h"

using AnimationSpeed = F32;

struct HealthBarSpriteAnimation
{
	ecs::Entity spritesheet;
	AnimationSpeed frame_length{ 1.0f };
	I32 current_frame{ 0 };

};

struct Sprite;
struct Spritesheet;

class HealthBarAnimationSystem
	: public ecs::System
	, public AccessStorage<Sprite>
	, public AccessStorage<Spritesheet>
	, public AccessEntityValidity
	, public MutAccessGroupStorage<Sprite, HealthBarSpriteAnimation, PlayerId>
	, public SignalProcessor<HealthUpdateSignal>
	, public SignalProcessor<OnStartSignal>
{
	virtual void process_signal(HealthUpdateSignal& signal);
	virtual void process_signal(OnStartSignal& signal);
};

