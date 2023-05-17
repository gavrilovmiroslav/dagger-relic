#pragma once

#include "GameComponents.h"
#include "Prelude.h"
#include "SDL_mixer.h"

#define AUDIO_VOLUME 30
#define BUTTON_SIZE 20
#define CHUNK_SIZE 1024
#define DELTA_VOLUME 3
#define FREQUENCY 48000
#define INITIAL_VOLUME (MIX_MAX_VOLUME/2)
#define NUM_OF_CHANNELS 2

enum class Sound
{
	ButtonClick,
	PushPlateActivated
};

class SoundManager
{
public:
	SoundManager();

	void play_music(U32 music_index) const;
	void play_sound(Sound sound) const;
	Bool toggle_music() const;

	~SoundManager();

private:
	DynamicArray<RawPtr<Mix_Music>> m_music;
	Map<String, RawPtr<Mix_Chunk>> m_sound_effects;

	void load_audio_from_file(String file_name);
};

struct SoundControlSystem
	: public ecs::System
	, public AllocateUnique<SoundManager>
	, public MutAccessUnique<SoundManager>
	, public MutAccessGroupStorage<Button, Position, SpriteAnimation>
	, public SignalProcessor<PushPlateActivatedSignal>
{
	using QueryButtons = MutAccessGroupStorage<Button, Position, SpriteAnimation>;
	U32 current_song = 0;

	virtual void process_signal(PushPlateActivatedSignal& signal);

	void on_tick() override;
};
