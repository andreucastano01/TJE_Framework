#pragma once
#include "extra/bass.h"
#include <map>
#include <string>
#include "framework.h"

class Audio {
public:
	// Handler for BASS_SampleLoad
	static HSAMPLE sample;

	Audio();	// Sample = 0 here
	~Audio(); 	// Free sample (BASS_SampleFree)

	// Play audio and return the channel
	HCHANNEL play(float volume);

	// Initialize BASS
	static bool Init(const char* filename);
	// Get from manager map
	static Audio* Get(const char* filename);
	// Play Manager API
	static HCHANNEL Play(const char* filename);
	static HCHANNEL Play3D(const char* filename, Vector3 position);
	// Stop sounds
	static bool Stop(HCHANNEL channel);
};
