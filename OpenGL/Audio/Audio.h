#ifndef AUDIO
#define AUDIO

#include "./src/fmod.hpp"
#include "./other/common.h"

#include <map>
#include <string>

using namespace std;

struct Sound {
    FMOD::Sound * sound;

    //Unique channel on what the sound will be played
    FMOD::Channel * channel;

    //Sound volume [0.0, 1.0]
    float volume;
};

class Audio {
public:
	//The FMOD system used for audio playback
	FMOD::System  * fmod_system;

	//The FMOD result var used for debugging errors within the FMOD system
	FMOD_RESULT     result;

	//FMOD version used for audio playback
	unsigned int    version;

	//By default 0 to avoid unnecessary specific driver specific data, by standard wav, mp3...
	void          * extradriverdata = 0;

	map<string, Sound> sounds;

	Audio();
	void changeAudio(string & command);
	void freeAudio();
};


#endif