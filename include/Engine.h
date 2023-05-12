#pragma once

#include "Config.h"
#include "Lifecycle.h"
#include <entt/entt.hpp>

#include <chrono>
#include <utility>
#include <algorithm>

using namespace std::chrono;

struct Time;

namespace core
{
	struct GameStartSignal {};
	struct GameEndSignal {};
	struct FrameDurationSignal { F32 frame_duration; };

	enum EngineDefaults
	{
		Standard2D,
	};

	class Engine
	{
	public:
		static Engine& get_instance()
		{
			static Engine instance;
			return instance;
		}

	public:
		Engine(Engine const&) = delete;
		void operator=(Engine const&) = delete;

		ecs::Registry registry;
		ecs::Dispatcher dispatcher;

	private:
		Engine() = default;

		bool should_quit = false;

		Config configuration;
		containers::DynamicArray<memory::SharedPtr<ecs::LifecycleTrait>> lifetimes;

		F32 time_delta = 1.0f / 60.0f;

		template<typename T>
		memory::SharedPtr<T>& access_type_indexed_resource()
		{
			static memory::SharedPtr<T> instance;
			return instance;
		}

		template<typename T>
		memory::SharedPtr<T>& set_type_indexed_resource(memory::SharedPtr<T> i)
		{
			auto& instance = access_type_indexed_resource<T>();
			instance = i;
			return instance;
		}

	public:
		friend struct ::Time;

		inline void quit()
		{
			should_quit = true;
		}

		inline const containers::DynamicArray<memory::SharedPtr<ecs::LifecycleTrait>>& get_lifetimes() const
		{
			return lifetimes;
		}

		inline Config& get_config()
		{
			return configuration;
		}

		void configure(RawString config_file, Size argumentCount, RawString arguments[])
		{
			configuration = override_config_with_command_line(load_config_from_ini_file(config_file), argumentCount, arguments);
		}

		template<typename S>
		void use(memory::RawPtr<S> mod)
		{
			lifetimes.push_back(memory::SharedPtr<S>{mod});
		}

		template<typename S, typename... Args>
		memory::SharedPtr<S> use(Args&&... args)
		{
			auto ptr = memory::make_shared_ptr<S>(std::forward<Args>(args)...);
			set_type_indexed_resource<S>(ptr);
			lifetimes.push_back(ptr);
			return ptr;
		}

		template<typename S>
		void setup()
		{
			set_type_indexed_resource<S>(memory::make_shared_ptr<S>());
		}

		template<typename S>
		memory::SharedPtr<S>& get()
		{
			memory::SharedPtr<S>& ptr = access_type_indexed_resource<S>();
			if (!ptr)
			{
				Logger::critical("Lifecycle {} not registered, but accessed through `Engine::get`.", typeid(S).name());
			}
			return ptr;
		}

		inline void frame()
		{
			static high_resolution_clock::time_point start_time;
			static high_resolution_clock::time_point end_time;
			static duration<F32> frame_duration;

			start_time = high_resolution_clock::now();
			for (auto& lifetime : lifetimes)
			{
				lifetime->on_tick();
			}
			end_time = high_resolution_clock::now();
			frame_duration = end_time - start_time;
			time_delta = frame_duration.count();
			dispatcher.trigger(FrameDurationSignal{ time_delta });
		}

		void run()
		{
			for (auto& lifetime : lifetimes)
			{
				lifetime->on_start();
			}

			dispatcher.trigger(GameStartSignal{});

			while (!should_quit)
			{
				frame();
			}

			dispatcher.trigger(GameEndSignal{});

			std::reverse(lifetimes.begin(), lifetimes.end());
			for (auto& lifetime : lifetimes)
			{
				lifetime->on_end();
			}
		}
	};
}

struct Time
{
	inline static const F32 delta_time()
	{
		return core::Engine::get_instance().time_delta;
	}
};
