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

#define POSTPROCESS_TEST_VIGNETTE 1u
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
