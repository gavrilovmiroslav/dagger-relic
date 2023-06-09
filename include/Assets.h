#pragma once

#include "Definitions.h"
#include "Access.h"

#include <fstream>

namespace core
{
	struct WindowingState;

	class AssetLoadStartSignal {};
	class AssetLoadEndSignal {};

	template<typename PhantomT>
	struct AssetUnloadingSignal
	{
		ecs::Entity id;
	};

	struct AbstractAssetLoader
		: public SignalProcessor<AssetLoadStartSignal>
	{
		friend class AssetModule;

		containers::Map<String, ecs::Entity> loaded_entity_mapping;

		virtual void load_assets() = 0;
		virtual Bool contains(String) const = 0;

		void process_signal(AssetLoadStartSignal&) override;

	protected:
		Size asset_type_id = 0;
	};

	class AssetModule
		: public ecs::System
		, public MutAccessUnique<WindowingState>
		, public SignalEmitter<AssetLoadStartSignal>
		, public SignalEmitter<AssetLoadEndSignal>
	{
		static containers::Map<Size, memory::SharedPtr<AbstractAssetLoader>> loaders;

		bool on_start();

	public:
		AssetModule() = default;

		template<typename T>
		static Size get_type_id()
		{
			return typeid(T).hash_code();
		}

		template<typename T>
		ecs::Entity get_asset(String path)
		{
			const auto id = AssetModule::template get_type_id<T>();
			assert(AssetModule::loaders.find(id) != AssetModule::loaders.end());

			const auto& loader = AssetModule::loaders.at(id);
			if (loader->contains(path))
			{
				return loader->loaded_entity_mapping.at(path);
			}
			else
			{
				return ecs::no_entity;
			}
		}

		template<typename T>
		Bool contains(String path) const
		{
			const auto id = AssetModule::template get_type_id<T>();
			assert(AssetModule::loaders.find(id) != AssetModule::loaders.end());

			const auto& loader = AssetModule::loaders.at(id);
			return loader->contains(path);
		}

		template<typename L>
		void add_loader()
		{
			add_loader(new L{});
		}

		template<typename L>
		void add_loader(memory::RawPtr<L> loader)
		{
			assert(loader->asset_type_id != 0);

			AssetModule::loaders.insert({
				loader->asset_type_id,
				memory::SharedPtr<L>(loader)
				});
		}
	};

	template<typename T>
	struct AssetLoader
		: public AbstractAssetLoader
	{
		virtual String get_default_asset_name() const = 0;

		String get_loader_path()
		{
			std::ifstream entry("data/entry.data");
			containers::Set<String> keys{};

			String line;
			while (std::getline(entry, line))
			{
				keys.insert(line);
			}
			entry.close();
			auto name = get_default_asset_name();
			return name + ".data";
		}

		virtual void load_assets() = 0;
		virtual ecs::Entity load_asset(String texture_name, String texture_path) = 0;
		virtual Bool contains(String) const = 0;

		AssetLoader()
		{
			asset_type_id = AssetModule::template get_type_id<T>();
		}
	};
}
