#pragma once

namespace ecs
{
	struct LifecycleTrait
	{
		virtual Bool on_start() { return true; };
		virtual void on_tick() {};
		virtual void on_end() {};

		// TODO: adding `on_pause()` and `on_resume()` and tying them together in `Engine.h` might be cool
	};
}