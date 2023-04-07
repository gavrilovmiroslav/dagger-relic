#include "Loaders.h"
#include "Assets.h"
#include "Definitions.h"
#include "Engine.h"

#include "Config.h"
#include "Logger.h"
#include "Rendering.h"
#include "SpriteRendering.h"
#include "Windowing.h"
#include "Textures.h"

#include <SDL.h>
#include <SDL_image.h>

#include <algorithm>

// Texture loader


TextureLoader::TextureLoader()
	: core::AssetLoader<Texture>()
{
}

memory::RawPtr<Texture> TextureLoader::load_texture(String path)
{
	auto& state = MutAccessUnique<core::WindowingState>::access_unique();
	auto* texture = new Texture();

	auto* surface = IMG_Load(path.c_str());
	texture->inner = SDL_CreateTextureFromSurface(state.renderer, surface);
	
	Logger::info("Loading complete");
	
	return texture;
}

String TextureLoader::get_default_asset_name() const
{
	return "textures";
}

ecs::Entity TextureLoader::load_asset(String texture_name, String texture_path)
{
	auto& registry = core::Engine::get_instance().registry;

	auto loaded = load_texture(texture_path);
	Bool is_found = contains(texture_name);

	if (loaded == nullptr)
	{
		if (is_found)
		{
			loaded_entity_mapping.erase(texture_name);
			Logger::error("Loading texture '{}' failed.", texture_path);
		}

		return ecs::no_entity;
	}

	if (loaded != nullptr)
	{
		if (!is_found)
		{
			auto entity = registry.create();
			registry.emplace<Texture>(entity, *loaded);
			loaded_entity_mapping.insert({ texture_name, entity });
			return entity;
		}
		else
		{
			auto entity = loaded_entity_mapping.at(texture_name);
			registry.replace<Texture>(entity, *loaded);
			return entity;
		}
	}

	return ecs::no_entity;
}

void TextureLoader::load_assets()
{
	auto& registry = core::Engine::get_instance().registry;
	auto asset_path = get_loader_path();

	std::ifstream textures("data/" + asset_path);

	containers::DynamicArray<String> entries;
	containers::Map<String, String> name_paths;
	containers::DynamicArray<String> to_remove;

	String line;
	String name, path;
	while (std::getline(textures, line))
	{
		std::stringstream sstream(line);
		sstream >> name >> path;
		entries.push_back(name);
		name_paths.insert({ name, path });
	}

	for (auto loaded : loaded_entity_mapping)
	{
		if (std::find(entries.begin(), entries.end(), loaded.first) == entries.end())
		{
			SignalEmitter<core::AssetUnloadingSignal<Texture>>::emit(core::AssetUnloadingSignal<Texture>{ loaded.second });
			registry.remove<Texture>(loaded.second);
		}

		to_remove.push_back(loaded.first);
	}

	for (auto del : to_remove)
	{
		loaded_entity_mapping.erase(del);
	}

	for (auto& [ name, path ] : name_paths)
	{
		auto texture_name = name;
		auto texture_path = "data/" + path;

		load_asset(texture_name, texture_path);
	}
}

void TextureLoader::process_signal(core::WindowShutdownSignal&)
{
	const auto& storage = AccessStorage<Texture>::access_storage();
	for (const auto& e : storage)
	{
		const auto& texture = storage.get<Texture>(e);
		SDL_DestroyTexture(texture.inner);
	}
}

Bool TextureLoader::contains(String name) const
{
	return loaded_entity_mapping.find(name) != loaded_entity_mapping.end();
}


// Sprite loader


String SpritesheetLoader::get_default_asset_name() const 
{
	return "spritesheets";
}

#include <cstring>
#include <optional>
#include <tuple>
#include <algorithm> 
#include <cctype>
#include <locale>

// trim from start (in place)
static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string& s) {
	rtrim(s);
	ltrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
	ltrim(s);
	return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
	rtrim(s);
	return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
	trim(s);
	return s;
}


void read_line(std::ifstream& input)
{
	String line;
	std::getline(input, line);
}

std::tuple<String, String> read_tuple(std::string line)
{
	auto split = line.find(":");
	auto first = trim_copy(line.substr(0, split));
	auto second = trim_copy(line.substr(split + 1, line.size()));

	return { first, second };
}

std::optional<String> read_pair(std::ifstream& input, std::string key)
{
	String line;

	std::getline(input, line);
	auto split = line.find(":");
	auto first = trim_copy(line.substr(0, split));
	if (first == key)
	{
		return trim_copy(line.substr(split + 1, line.size()));
	}
	else
	{
		return std::nullopt;
	}
}

int read_int(std::string str)
{
	int a = 0;
	sscanf(str.c_str(), "%d", &a);
	return a;
}

std::array<int, 4> read_4ints(std::string str)
{
	int a = 0, b = 0, c = 0, d = 0;
	sscanf(str.c_str(), "%d, %d, %d, %d", &a, &b, &c, &d);
	return std::array<int, 4>{ a, b, c, d };
}

std::array<int, 2> read_2ints(std::string str)
{
	int a = 0, b = 0;
	sscanf(str.c_str(), "%d, %d", &a, &b);
	return std::array<int, 2>{ a, b };
}

std::array<float, 2> read_2floats(std::string str)
{
	float a = 0.0f, b = 0.0f;
	sscanf(str.c_str(), "%f, %f", &a, &b);
	return std::array<float, 2>{ a, b };
}

std::array<float, 4> read_4floats(std::string str)
{
	float a = 0.0f, b = 0.0f, c = 0.0f, d = 0.0f;
	sscanf(str.c_str(), "%f, %f, %f, %f", &a, &b, &c, &d);
	return std::array<float, 4>{ a, b, c, d };
}

void SpritesheetLoader::load_assets() 
{
	auto& registry = core::Engine::get_instance().registry;
	auto asset_path = get_loader_path();
	
	containers::DynamicArray<String> entries;
	containers::DynamicArray<String> to_remove;

	std::ifstream entry("data/" + asset_path);	

	String line;
	while (std::getline(entry, line))
	{
		entries.push_back(line);
	}
	entry.close();

	for (auto loaded : loaded_entity_mapping)
	{
		if (std::find(entries.begin(), entries.end(), loaded.first) == entries.end())
		{
			emit(core::AssetUnloadingSignal<Spritesheet>{ loaded.second });
			registry.remove<Spritesheet>(loaded.second);
		}

		to_remove.push_back(loaded.first);
	}
	
	for (auto del : to_remove)
	{
		loaded_entity_mapping.erase(del);
	}

	for (auto& key : entries)
	{
		fs::Dir sprite_full_path{ key };

		const auto& [ name, path ] = read_tuple(key);
		auto sheet = load_asset(name, "data/" + path);
		loaded_entity_mapping.insert({ name, sheet });
	}
}

ecs::Entity SpritesheetLoader::load_asset(String spritesheet_name, String spritesheet_path)
{
	fs::Dir path{ spritesheet_path };
	if (!path.exists())
	{
		return ecs::no_entity;
	}

	std::ifstream input(spritesheet_path);		
	
	geometry::Vec2 cell_count;

	auto texture = AccessSystem<core::AssetModule>::access_system()->
		get_asset<Texture>(read_pair(input, "texture").value());

	assert(texture != ecs::no_entity);

	const auto& dimensions = read_2ints(read_pair(input, "dimensions").value());

	cell_count.x = dimensions[0];
	cell_count.y = dimensions[1];

	auto sprite_count = read_int(read_pair(input, "sprites").value());

	Spritesheet sheet;

	auto& registry = core::Engine::get_instance().registry;

	I32 index = 0;
	
	for (int i = 0; i < sprite_count; i++)
	{
		read_line(input);
		const auto& clip = read_4ints(read_pair(input, "clip").value());
		const auto& pivot = read_2floats(read_pair(input, "pivot").value());
		const auto& scale = read_2floats(read_pair(input, "scale").value());

		Sprite sprite;
		sprite.texture = texture;

		I32 x{ clip[0] };
		I32 y{ clip[1] };
		I32 w{ clip[2] };
		I32 h{ clip[3] };
		sprite.clip = geometry::Rect{ x, y, w, h };
		
		sprite.pivot.x = pivot[0];
		sprite.pivot.y = pivot[1];

		auto entity = registry.create();
		registry.emplace<Sprite>(entity, sprite);
		registry.emplace<I32>(entity, index);
		index++;

		sheet.sprites.push_back(entity);
	}

	auto sheet_entity = registry.create();
	registry.emplace<Spritesheet>(sheet_entity, sheet);
	input.close();

	return sheet_entity;
}

SpritesheetLoader::SpritesheetLoader() 
{}

Bool SpritesheetLoader::contains(String name) const
{
	return loaded_entity_mapping.find(name) != loaded_entity_mapping.end();
}


// Other loaders
