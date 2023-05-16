// #include "Common.h"
#include "TimeRender.h"
#include <iostream>
#include <random>

#define RANGE_X 801
#define RANGE_Y 601

TimeRender::TimeRender(String text, U32 font_size)
	: text(text), font_size(font_size)
{
	font = TTF_OpenFont("./data/fonts/Minecraft.ttf", font_size);

	if (!font)
	{
		spdlog::warn("Error loading font: {}", TTF_GetError());
	}
}

void TimeRender::init()
{
	if (TTF_Init() < 0)
	{
		spdlog::error("Error intializing SDL_ttf: {}", TTF_GetError());
	}
}

void TimeRender::deinit()
{
	TTF_Quit();
}

auto start = std::chrono::high_resolution_clock::now(); // get current time
int elapsed_time_ms = 0;
bool hasSpawnEnemies = false;
int num;
int numSpawnedEnemies = 1;

void TimeRenderControlSystem::on_tick()
{

	for (auto &&[entity, time_render, position, sprite] : access_storage().each())
	{
		sprite.depth = -7;
		if (!time_render.font)
		{
			continue;
		}

		auto end = std::chrono::high_resolution_clock::now(); // get current time
		elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		int minutes = elapsed_time_ms / (1000 * 60);
		int seconds = (elapsed_time_ms / 1000);
		int milliseconds = elapsed_time_ms;
		String time = "" + std::to_string(minutes) + ":" + std::to_string(seconds) + ":" + std::to_string(milliseconds);
		// String time = "Time elapsed: " + std::to_string(minutes) + ":" +  std::to_string(seconds);
		time_render.text = time;

		if ((seconds + 5) % 10 == 0)
		{
			time_render.text_color = {238, 38, 38};
		}

		if (seconds % 10 == 0)
		{

			time_render.text_color = {0, 0, 0};

			if (!hasSpawnEnemies)
			{

				for (int i = 1; i <= numSpawnedEnemies; i++)
				{
					num = rand() % RANGE_X;
					auto enemy2 = spawn()
									.with<Sprite>(ecs::no_entity)
									.with<Health>(100)
									.with<SpriteAnimation>(Spritesheet::get_by_name("Golem/Golem1_idle"))
									.with<Position>(geometry::Vec2{num, 0})
									.with<Visibility>(true)
									.with<Flip>(None)
									.with<AnimationSpeedController>(15.0f)
									.with<Enemy>(geometry::Vec2{1, 0}, 90.0f)
									.with<EnemyFSMInstance>("Idle")
									.done();

					num = rand() % RANGE_X;
					auto enemy3 = spawn()
									.with<Sprite>(ecs::no_entity)
									.with<Health>(100)
									.with<SpriteAnimation>(Spritesheet::get_by_name("Golem/Golem1_idle"))
									.with<Position>(geometry::Vec2{num, 600})
									.with<Visibility>(true)
									.with<Flip>(None)
									.with<AnimationSpeedController>(15.0f)
									.with<Enemy>(geometry::Vec2{1, 0}, 90.0f)
									.with<EnemyFSMInstance>("Idle")
									.done();

					num = rand() % RANGE_Y;
					auto enemy4 = spawn()
									.with<Sprite>(ecs::no_entity)
									.with<Health>(100)
									.with<SpriteAnimation>(Spritesheet::get_by_name("Golem/Golem1_idle"))
									.with<Position>(geometry::Vec2{0, num})
									.with<Visibility>(true)
									.with<Flip>(None)
									.with<AnimationSpeedController>(15.0f)
									.with<Enemy>(geometry::Vec2{1, 0}, 90.0f)
									.with<EnemyFSMInstance>("Idle")
									.done();

					num = rand() % RANGE_Y;
					auto enemy5 = spawn()
									.with<Sprite>(ecs::no_entity)
									.with<Health>(100)
									.with<SpriteAnimation>(Spritesheet::get_by_name("Golem/Golem1_idle"))
									.with<Position>(geometry::Vec2{800, num})
									.with<Visibility>(true)
									.with<Flip>(None)
									.with<AnimationSpeedController>(15.0f)
									.with<Enemy>(geometry::Vec2{1, 0}, 90.0f)
									.with<EnemyFSMInstance>("Idle")
									.done();
				}
				numSpawnedEnemies++;
			}
			hasSpawnEnemies = true;
		}
		else
		{
			hasSpawnEnemies = false;
		}

		auto &state = MutAccessUnique<WindowingState>::access_unique();
		SDL_Surface *text_surf = TTF_RenderText_Solid(time_render.font, time_render.text.c_str(), time_render.text_color);
		SDL_Texture *texture = SDL_CreateTextureFromSurface(state.renderer, text_surf);
		SDL_Rect dest;
		dest.x = position.xy.x;
		dest.y = position.xy.y;
		dest.w = text_surf->w;
		dest.h = text_surf->h;
		SDL_RenderCopy(state.renderer, texture, nullptr, &dest);
		SDL_DestroyTexture(texture);
		SDL_FreeSurface(text_surf);
		SDL_RenderPresent(state.renderer);
	}
}