#pragma once

#include "Config.h"
#include "Definitions.h"
#include "Engine.h"

struct AccessTrait 
{
	static containers::Map<Size, ecs::Entity> unique_resources;

	template<typename T>
	static Size get_unique_resource_id()
	{
		return typeid(T).hash_code();
	}

	template<typename T>
	static inline bool unique_resources_contain()
	{
		auto key = get_unique_resource_id<T>();
		return unique_resources.find(key) != unique_resources.end();
	}
};

template<typename T>
struct AccessSystem : public AccessTrait
{
	memory::SharedPtr<T>& access_system() const
	{
		return core::Engine::get_instance().get<T>();
	}
};

template<typename T>
struct AllocateUnique : public AccessTrait
{
	AllocateUnique()
	{
		auto& registry = core::Engine::get_instance().registry;
		assert(!AccessTrait::unique_resources_contain<T>());
		auto key = get_unique_resource_id<T>();
		auto entity = registry.create();
		registry.emplace_or_replace<T>(entity);
		AccessTrait::unique_resources.insert({ key, entity });
	}
};

template<typename T>
struct MutAccessUnique : public AccessTrait
{
	T& access_unique() const
	{
		auto& registry = core::Engine::get_instance().registry;
		assert(AccessTrait::unique_resources_contain<T>());

		auto key = get_unique_resource_id<T>();
		return registry.get<T>(unique_resources[key]);
	}
};

template<typename T>
struct AccessUnique : public AccessTrait
{
	const T& access_unique() const
	{
		auto& registry = core::Engine::get_instance().registry;
		assert(AccessTrait::unique_resources_contain<T>());

		auto key = get_unique_resource_id<T>();
		return registry.get<T>(unique_resources[key]);
	}
};

template<typename T>
struct AccessComponentById : public AccessTrait
{
	const T& get(ecs::Entity e) const 
	{
		return core::Engine::get_instance().registry.get<const T>(e);
	}
};

template<typename T>
struct MutAccessComponentById : public AccessTrait
{
	T& get(ecs::Entity e) const
	{
		return core::Engine::get_instance().registry.get<T>(e);
	}
};

template<typename T>
struct AccessStorage : public AccessTrait
{
	auto access_storage() const 
	{
		return core::Engine::get_instance().registry.view<const T>();
	}
};

template<typename T>
struct MutAccessStorage : public AccessTrait
{
	auto access_storage() const
	{
		return core::Engine::get_instance().registry.view<T>();
	}
};

template<typename... Ts>
struct AccessGroupStorage : public AccessTrait
{
	auto access_storage() const
	{
		return core::Engine::get_instance().registry.view<const Ts...>();
	}
};

template<typename... Ts>
struct MutAccessGroupStorage : public AccessTrait
{
	auto access_storage() const
	{
		return core::Engine::get_instance().registry.view<Ts...>();
	}
};

template<typename T>
struct SignalEmitter : public AccessTrait
{
	void emit(T& signal) const
	{
		core::Engine::get_instance().dispatcher.trigger(signal);
	}
};

template<typename T>
struct SignalProcessor : public AccessTrait
{
	virtual void process_signal(T& signal) = 0;

	SignalProcessor()
	{
		core::Engine::get_instance().dispatcher.sink<T>().connect<&SignalProcessor<T>::process_signal>(this);
	}
};

struct [[nodiscard]] EntitySpawnBuilder
{
	friend struct EntitySpawner;

private:
	ecs::Entity entity;

	EntitySpawnBuilder(ecs::Entity entity)
		: entity{entity}
	{}

public:

	template<typename T, typename... Args>
	EntitySpawnBuilder with(Args&&... args)
	{
		core::Engine::get_instance().registry.emplace<T>(entity, std::forward<Args>(args)...);
		return *this;
	}

	ecs::Entity done() const
	{
		return entity;
	}
};

struct EntitySpawner : public AccessTrait
{
	EntitySpawnBuilder spawn() const
	{
		return EntitySpawnBuilder{ core::Engine::get_instance().registry.create() };
	}

	void despawn(ecs::Entity entity) const
	{
		core::Engine::get_instance().registry.destroy(entity);
	}
};

struct EntityModifier : public AccessTrait
{
	template<typename T, typename... Args>
	void add_component(ecs::Entity entity, Args&&... args)
	{
		core::Engine::get_instance().registry.emplace<T>(entity, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	void replace_component(ecs::Entity entity, Args&&... args)
	{
		core::Engine::get_instance().registry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
	}

	template<typename T>
	void remove_component(ecs::Entity entity)
	{
		core::Engine::get_instance().registry.remove<T>(entity);
	}
};

namespace ecs
{
	struct System
		: public LifecycleTrait
		, public EntitySpawner
		, public EntityModifier {};
}

struct Game
	: public SignalProcessor<core::GameStartSignal>
	, public SignalProcessor<core::GameEndSignal>
	, public EntitySpawner
	, public EntityModifier
{
	virtual void on_start() {};
	virtual void on_end() {};

	void process_signal(core::GameStartSignal&) override
	{
		on_start();
	}

	void process_signal(core::GameEndSignal&) override
	{
		on_end();
	}
};
