#pragma once

#include "Access.h"
#include "Definitions.h"
#include "Geometry.h"

using AnimationSpeed = F32;

struct SpriteAnimation
{
	ecs::Entity spritesheet;
	AnimationSpeed frame_length{ 1.0f };
	I32 current_frame{ 0 };

	void change_to(String animation_name);
};

struct Sprite;
struct Spritesheet;

namespace core {
	struct PostRenderSignal;
}

struct AnimationSpeedController
{
	F32 animation_speed;
};

class AnimationModule
	: public ecs::System
	, public AccessStorage<Sprite>
	, public AccessStorage<Spritesheet>
	, public MutAccessGroupStorage<Sprite, SpriteAnimation, AnimationSpeedController>
	, public SignalProcessor<core::PostRenderSignal>
{
	void process_signal(core::PostRenderSignal&) override;
};
