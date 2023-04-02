#pragma once

#include "Access.h"
#include "Definitions.h"
#include "Geometry.h"

struct OverlayLine
{
	geometry::Vec2 start;
	geometry::Vec2 end;
	uint32_t       colour;

	OverlayLine(geometry::Vec2 start, geometry::Vec2 end, uint32_t colour)
		: start(start)
		, end(end)
		, colour(colour)
	{ }
};

struct OverlayText
{
	std::string    text;
	geometry::Vec2 position;
};

struct OverlayTest
{
	uint32_t magic = 0; /* Define test number. */

	OverlayTest(uint32_t magic)
		: magic(magic)
	{ }
};

namespace core
{
	struct RenderSignal;
	struct Render2Signal;
}

class OverlayLineRenderingModule
	: public ecs::System
	, public AccessStorage<OverlayLine>
	, public SignalProcessor<core::Render2Signal>
{
	void process_signal(core::Render2Signal& signal) override;
};

class OverlayTextRenderingModule
	: public ecs::System
	, public AccessStorage<OverlayText>
	, public SignalProcessor<core::Render2Signal>
{
	void process_signal(core::Render2Signal& signal) override;
};

class OverlayTestRenderingModule
	: public ecs::System
	, public AccessStorage<OverlayTest>
	, public SignalProcessor<core::Render2Signal>
{
	void process_signal(core::Render2Signal& signal) override;
};
