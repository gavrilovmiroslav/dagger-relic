#include "SoundManager.h"

// Audio is initialized in Windowing.cpp
SoundManager::SoundManager()
{
	load_audio_from_file("data/music/music_list.txt");
	play_music(0u);
}

void SoundManager::load_audio_from_file(String file_name)
{
	IFStream file;
	file.open(file_name.c_str());

	if (!file)
	{
		spdlog::error("Failed to open file!");
		return;
	}

	U32 line_counter = 0;
	U32 music_counter = 0;

	String audio_key, audio_path;
	while (file >> audio_key >> audio_path)
	{
		if (line_counter == 0)
		{
			music_counter = atoi(audio_key.c_str());
			line_counter++;
			continue;
		}

		if (line_counter <= music_counter)
		{
			if (Mix_OpenAudio(FREQUENCY, MIX_DEFAULT_FORMAT, NUM_OF_CHANNELS, CHUNK_SIZE) == -1)
			{
				spdlog::error("Cannot open audio! {}!", Mix_GetError());
    			}

			m_music.push_back(Mix_LoadMUS(audio_path.c_str()));
			if (!m_music.back())
			{
				spdlog::error("Cannot load audio! {}", Mix_GetError());
			}
		}
		else
		{
			if (Mix_OpenAudio(FREQUENCY, MIX_DEFAULT_FORMAT, NUM_OF_CHANNELS, CHUNK_SIZE) == -1)
			{
				spdlog::error("Cannot open sound effect! {}!", Mix_GetError());
			}

			m_sound_effects[audio_key] = Mix_LoadWAV(audio_path.c_str());

			if (!m_sound_effects[audio_key])
			{
				spdlog::error("Failed to load sound effect! {}!", Mix_GetError());
			}
		}

		line_counter++;
	}

	file.close();
}

SoundManager::~SoundManager()
{
	for (auto& music : m_music)
	{
		Mix_FreeMusic(music);
	}

	for (auto& entry : m_sound_effects)
	{
		Mix_FreeChunk(entry.second);
	}

    	Mix_Quit();
}

void SoundManager::play_music(U32 music_index) const
{
	if (music_index >= 0 && music_index < m_music.size())
	{
		if (m_music[music_index])
		{
			// -1 => looped play
			Mix_PlayMusic(m_music[music_index], -1);
			Mix_VolumeMusic(INITIAL_VOLUME);
		}
	}
}

Bool SoundManager::toggle_music() const
{
	if (Mix_PausedMusic() == 1)
	{
		Mix_ResumeMusic();
		return true;
	}
	else if (Mix_PausedMusic() == 0)
	{
		Mix_PauseMusic();
		return false;
	}
}

void SoundManager::play_sound(Sound sound_type) const
{
	switch (sound_type)
	{
		case Sound::ButtonClick:
		{
			Mix_VolumeChunk(m_sound_effects.at("button_click"), AUDIO_VOLUME);
			Mix_PlayChannel(-1, m_sound_effects.at("button_click"), 0);
		} break;
		case Sound::PushPlateActivated:
		{
			Mix_VolumeChunk(m_sound_effects.at("push_plate"), AUDIO_VOLUME);
			Mix_PlayChannel(-1, m_sound_effects.at("push_plate"), 0);
		} break;
	}
}

void SoundControlSystem::process_signal(PushPlateActivatedSignal& signal)
{
	auto&& music = MutAccessUnique<SoundManager>::access_unique();
	music.play_sound(Sound::PushPlateActivated);
}

void SoundControlSystem::on_tick()
{
	const auto& mouse = MouseState::get();
	Bool is_playing = true;

	auto&& music = MutAccessUnique<SoundManager>::access_unique();

	for (auto&& [button_entity, button, position, sprite_animation] : QueryButtons::access_storage().each())
	{
		I32 position_x = mouse.get_mouse_position().x;
		I32 position_y = mouse.get_mouse_position().y;

		Bool valid_position_x = position_x >= position.xy.x && position_x <= position.xy.x + BUTTON_SIZE;
		Bool valid_position_y = position_y >= position.xy.y && position_y <= position.xy.y + BUTTON_SIZE;

		if (valid_position_x && valid_position_y && mouse.get_mouse_button_left() == MouseEventState::DownNow)
		{
			music.play_sound(Sound::ButtonClick);
			switch (button.type)
			{
				case ButtonType::PauseMusic:
				{
					is_playing = music.toggle_music();
					if (is_playing)
					{
						sprite_animation.change_to("pyramidplunder/audio_active");
					}
					else
					{
						sprite_animation.change_to("pyramidplunder/audio_not_active");
					}
				} break;
				case ButtonType::PlayNext:
				{
					music.play_music(++current_song);
				} break;
				case ButtonType::PlayPrevious:
				{
					music.play_music(--current_song);
				} break;
			}
		}
	}
}
