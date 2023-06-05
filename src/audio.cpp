#include "audio.h"
#include <cassert>
#include <iostream>

// Manager stuff
// The map!
static std::map<std::string, Audio*> sAudiosLoaded;

unsigned long Audio::sample = 0;

Audio::Audio() {
	sample = 0;
}

Audio::~Audio() {
	BASS_SampleFree(sample);
}

HCHANNEL Audio::play(float volume) {
	HCHANNEL hSampleChannel = BASS_SampleGetChannel(sample, false);
	// Establecer el volumen
	BASS_ChannelSetAttribute(hSampleChannel, BASS_ATTRIB_VOL, volume);
	// Reproducir el canal
	BASS_ChannelPlay(hSampleChannel, true);
	// Devolver el canal de muestra
	return hSampleChannel;
}

// Initialize BASS
bool Audio::Init(const char* filename) {
	if (BASS_Init(-1, 44100, 0, 0, NULL) == false) {
		std::cout << "Error loading Bass" << std::endl;
	}

	// Load sample from disk
	// flags: BASS_SAMPLE_LOOP, BASS_SAMPLE_3D, ...
	sample = BASS_SampleLoad(
		false,  			// From internal memory
		filename, 	// Filepath
		0,				// Offset
		0,				// Length
		3,				// Max playbacks
		0 				// Flags
	);

	if (sample == 0) // Error loading
		return false;

	return true;
}

// Get from manager map
Audio* Audio::Get(const char* filename) {
	assert(filename);

	//check if loaded
	auto it = sAudiosLoaded.find(filename);
	if (it != sAudiosLoaded.end())
		return it->second;

	//load it
	Audio* audio = new Audio();
	if (!audio->Init(filename))
	{
		delete audio;
		return NULL;
	}

	return audio;
}

// Play Manager API
HCHANNEL Audio::Play(const char* filename) {
	Audio* a = Audio::Get(filename);
	HCHANNEL hSampleChannel;
	hSampleChannel = BASS_SampleGetChannel(a->sample, false);
	BASS_ChannelSetAttribute(hSampleChannel, BASS_ATTRIB_VOL, 0.05f);
	BASS_ChannelPlay(hSampleChannel, true);
	return hSampleChannel;
}

HCHANNEL Audio::Play3D(const char* filename, Vector3 position) {
	Audio* a = Audio::Get(filename);
	HCHANNEL hSampleChannel;
	hSampleChannel = BASS_SampleGetChannel(a->sample, false);
	BASS_ChannelSetAttribute(hSampleChannel, BASS_ATTRIB_VOL, 0.05f);

	BASS_3DVECTOR dposition;
	dposition.x = position.x;
	dposition.y = position.y;
	dposition.z = position.z;
	BASS_Set3DPosition(&dposition, NULL, NULL, NULL);
	BASS_ChannelSet3DPosition(hSampleChannel, &dposition, NULL, NULL);

	// Apply changes to 3D system
	BASS_Apply3D();

	BASS_ChannelPlay(hSampleChannel, true);

	return hSampleChannel;
}

// Stop sounds
bool Audio::Stop(HCHANNEL channel) {
	return BASS_ChannelStop(channel);
}