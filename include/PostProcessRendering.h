#pragma once

#include "Access.h"
#include "Definitions.h"
#include "Geometry.h"

struct PostProcessLine
{
	geometry::Vec2 start;
	geometry::Vec2 end;
	U32            colour;

	PostProcessLine(geometry::Vec2 start, geometry::Vec2 end, U32 colour)
		: start(start)
		, end(end)
		, colour(colour)
	{ }
};

struct PostProcessText
{
	String         text;
	geometry::Vec2 position;
};

struct PostProcessRectangle
{
	I32 x, y, w, h;
};

#define POSTPROCESS_TEST_VIGNETTE     1u
#define POSTPROCESS_TEST_FADE         2u
#define POSTPROCESS_TEST_LIGHTMAP     3u
#define POSTPROCESS_TEST_ENVIRONMENT  4u
#define POSTPROCESS_TEST_DYNAMICLIGHT 5u
#define POSTPROCESS_TEST_UIFRAME      6u
#define POSTPROCESS_TEST_TRACK        7u

#define DECAL_TEST             0u
#define DECAL_FOOTPRINT_BIPED1 1u
#define DECAL_FOOTPRINT_BIPED2 2u
#define DECAL_BOXGRIND         3u

extern F32  postprocess_fade_timer;
extern Bool postprocess_fade_force;
extern U32  dynamiclight_x;
extern U32  dynamiclight_y;

struct Lightmap_Block
{
	U32 x, y, w, h;
};

void Lightmap_Calculate(std::vector<Lightmap_Block> &block);
void Decal_Stamp(U32 x, U32 y, U32 what = DECAL_TEST);
void Decal_Clear(void);

struct PostProcessTest
{
	U32 magic = 0; /* Define test number. */

	PostProcessTest(U32 magic)
		: magic(magic)
	{ }
};

namespace core
{
	struct PostProcessRenderSignal;
}

class PostProcessLineRenderingModule
	: public ecs::System
	, public AccessStorage<PostProcessLine>
	, public SignalProcessor<core::PostProcessRenderSignal>
{
	void process_signal(core::PostProcessRenderSignal& signal) override;
};

class PostProcessTextRenderingModule
	: public ecs::System
	, public AccessStorage<PostProcessText>
	, public SignalProcessor<core::PostProcessRenderSignal>
{
	void process_signal(core::PostProcessRenderSignal& signal) override;
};

class PostProcessTestRenderingModule
	: public ecs::System
	, public AccessStorage<PostProcessTest>
	, public SignalProcessor<core::PostProcessRenderSignal>
{
	void process_signal(core::PostProcessRenderSignal& signal) override;
};

class PostProcessRectangleRenderingModule
	: public ecs::System
	, public AccessStorage<PostProcessRectangle>
	, public SignalProcessor<core::PostProcessRenderSignal>
{
	void process_signal(core::PostProcessRenderSignal& signal) override;
};
