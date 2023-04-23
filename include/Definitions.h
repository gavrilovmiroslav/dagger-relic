#pragma once

#include <entt/entt.hpp>
#include <filesystem>
#include <functional>
#include <future>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <initializer_list>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <SDL.h>
#include <unordered_map>
#include <variant>
#include <vector>
#include <type_traits>

using Size = std::size_t;
using Void = std::nullptr_t;
//using Unit = std::monostate;
using Bool = bool;
using Char = char;

template<typename T>
struct Pair
{
	T x;
	T y;
};

using U8 = std::uint8_t;
using U16 = std::uint16_t;
using U32 = std::uint32_t;
using U64 = std::uint64_t;

using I8 = std::int8_t;
using I16 = std::int16_t;
using I32 = std::int32_t;
using I64 = std::int64_t;

using F32 = std::float_t;
using F64 = std::double_t;

using tid = std::uint32_t;

namespace geometry 
{
	using Vec2 = glm::vec2;
	using Vec3 = glm::vec3;
	using Vec4 = glm::vec4;

	using Mat4x4 = glm::mat4x4;
	using Rect = SDL_Rect;
}

namespace containers
{
	template<typename T, I32 N>
	using StaticArray = std::array<T, N>;

	template<typename T>
	using DynamicArray = std::vector<T>;

	template<typename T>
	using Queue = std::queue<T>;

	template<typename K, typename V>
	using Map = std::unordered_map<K, V>;

	template<typename T>
	using ArgList = std::initializer_list<T>;

	template<typename T>
	using Set = std::unordered_set<T>;

	template<typename T, typename IndexSize = I8, IndexSize N = 10>
	struct SmallBuffer
	{
		SmallBuffer()
			: free{}
			, entries{}
		{
			for (IndexSize i = 0; i < N; i++)
			{
				free[i] = true;
			}
		}

		~SmallBuffer() = default;

		void add(T t)
		{
			for (IndexSize i = 0; i < N; i++)
			{
				if (free[i])
				{
					free[i] = false;
					entries[i] = t;
					break;
				}
			}
		}

		void remove(T t)
		{
			for (IndexSize i = 0; i < N; i++)
			{
				if (!free[i] && entries[i] == t)
				{
					free[i] = true;
					break;
				}
			}
		}

		Bool contains(T& t) const
		{
			for (IndexSize i = 0; i < N; i++)
			{
				if (!free[i] && entries[i] == t)
					return true;
			}

			return false;
		}

		void clear()
		{
			for (IndexSize i = 0; i < N; i++)
			{
				free[i] = true;
			}
		}
	private:
		StaticArray<bool, N> free;
		StaticArray<T, N> entries;
	};
}

template<typename T>
using Ref = T&;

using URawString = unsigned char*;
using RawString = char*;
using LiteralString = const char*;

using String = std::string;

template<typename T>
using Function = std::function<T>;

namespace memory
{
	template<typename T>
	using RawPtr = T*;

	template<typename T>
	using UniquePtr = std::unique_ptr<T>;

	template<typename T, typename Deleter>
	using UniquePtrWithDeleter = std::unique_ptr<T, Deleter>;

	template<typename T>
	const auto make_unique_ptr = std::make_unique<T>;

	template<typename T>
	const auto make_shared_ptr = std::make_shared<T>;

	template<typename T>
	using SharedPtr = std::shared_ptr<T>;

	template<typename T>
	using WeakPtr = std::weak_ptr<T>;
}

namespace async
{
	template<typename T>
	using Future = std::future<T>;

	using Task = std::future<void>;

	using Mutex = std::mutex;

	using Lock = std::lock_guard<Mutex>;
}

namespace bus
{
	using Event = SDL_Event;

	// TODO: this is a sad thing because all `SDL_Scancode` entries have the prefix `SDL_` :(
	using Key = SDL_Scancode;

	// C++11 has function aliasing now, we should use it!
	const auto PollEvent = SDL_PollEvent;
}

namespace ecs
{
	using Registry = entt::registry;
	using Dispatcher = entt::dispatcher;

	using Entity = entt::entity;

	const auto no_entity = entt::null;
}

namespace fs
{
	using Dir = std::filesystem::directory_entry;

	using DirIter = std::filesystem::directory_iterator;

	inline String get_simple_name(Dir& path)
	{
		return path.path().filename().string();
	}
}
