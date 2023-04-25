#pragma once

#include "GameComponents.h"
#include "Prelude.h"
#include "SDL_mixer.h"

#define CHUNK_SIZE 1024
#define DELTA_VOLUME 3
#define FREQUENCY 48000
#define INITIAL_VOLUME 100
#define NUM_OF_CHANNELS 2

class SoundManager
{
public:
	SoundManager(String file_name);

	void play_sound() const;
	void toggle_sound() const;

	~SoundManager();

private:
	RawPtr<Mix_Music> m_music;
};

struct SoundControlSystem
	: public ecs::System
	, public MutAccessGroupStorage<SoundManager, KeyBinding>

{
	void on_tick() override;
};
