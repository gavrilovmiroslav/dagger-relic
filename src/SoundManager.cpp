#include "SoundManager.h"

// Audio is initialized in Windowing.cpp
SoundManager::SoundManager(String file_name)
{
	if (Mix_OpenAudio(FREQUENCY, MIX_DEFAULT_FORMAT, NUM_OF_CHANNELS, CHUNK_SIZE) == -1)
	{
		spdlog::error("Cannot open audio! {}!", Mix_GetError());
    	}

	m_music = Mix_LoadMUS(file_name.c_str());
	if (!m_music)
    	{
        	spdlog::error("Failed to load file that contains audio! {}!", Mix_GetError());
    	}

	play_sound();
}

SoundManager::~SoundManager()
{
	Mix_FreeMusic(m_music);
    	Mix_Quit();
}

void SoundManager::play_sound() const
{
	if (m_music)
	{
		// -1 => looped play
		Mix_PlayMusic(m_music, -1);
		Mix_VolumeMusic(INITIAL_VOLUME);
	}
}

void SoundManager::toggle_sound() const
{
	if (Mix_PausedMusic() == 1)
	{
		spdlog::info("MUSIC PAUSED");
		Mix_ResumeMusic();
	}
	else if (Mix_PausedMusic() == 0)
	{
		spdlog::info("MUSIC PLAYING...");
		Mix_PauseMusic();
	}
}

void SoundControlSystem::on_tick()
{
	const auto& keys = KeyState::get();
	for (auto&& [music_entity, music, key_binding] : access_storage().each())
	{
		if (keys.is_pressed(key_binding.toggle_sound))
		{
			spdlog::info("PRESSED");
			music.toggle_sound();
		}
	}
}
