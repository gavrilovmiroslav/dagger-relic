#include "Assets.h"

using namespace core;

// in AbstractAssetLoader:

containers::Map<Size, memory::SharedPtr<AbstractAssetLoader>> AssetModule::loaders = {};

void AbstractAssetLoader::process_signal(AssetLoadStartSignal&)
{
	load_assets();
}

// in AssetModule:

bool AssetModule::on_start()
{
	SignalEmitter<AssetLoadStartSignal>::emit(AssetLoadStartSignal{});

	// TODO: these two events one after the other is okay if loaders are synchronous,
	//	 see what we can do about this if we want to go asynchronous

	SignalEmitter<AssetLoadEndSignal>::emit(AssetLoadEndSignal{});

	return true;
}
