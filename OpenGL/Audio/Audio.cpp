#include <iostream>
#include <string>
#include <iostream>
#include <map>

#include "./src/fmod.hpp"
#include "./other/common.h"

#include "Audio.h"
using namespace std;

Audio::Audio() {
	this->sounds["obstacle1.mp3"] = Sound{0, NULL, 1.0};
	this->sounds["white_stripes.mp3"] = Sound{0, NULL, 1.0};
	this->sounds["blaster_shot.wav"] = Sound{0, NULL, 0.15};
	this->sounds["rocket_engine.wav"] = Sound{0, NULL, 0.25};

	/* Create a System object and initialize */
	this->result = FMOD::System_Create(&this->fmod_system);
	ERRCHECK(this->result);

	this->result = this->fmod_system->getVersion(&this->version);
	ERRCHECK(this->result);

	if(this->version < FMOD_VERSION) {
	    Common_Fatal("FMOD lib version doesn't match header version ", this->version, FMOD_VERSION);
	}


	//Allocate sounds.size() channels for the full possibility that every sound can be played in the same time
	this->result = this->fmod_system->init(this->sounds.size(), FMOD_INIT_NORMAL, this->extradriverdata);
	ERRCHECK(this->result);

	map<string, Sound>::iterator it;
	for(it = this->sounds.begin(); it != this->sounds.end(); it++) {
	    this->result = this->fmod_system->createSound(Common_MediaPath(it->first.c_str()), FMOD_DEFAULT, 0, &it->second.sound);
	    ERRCHECK(this->result);
	};

	//Setting up LOOP turned on for the rocket_engine
	this->result = this->sounds["rocket_engine.wav"].sound->setMode(FMOD_LOOP_NORMAL);
	ERRCHECK(this->result);
}

void Audio::changeAudio(string & command) {

    if(command.length() >= 5) {
        string action = command.substr(0, 5);

        if(command.length() != 13) {
            if(!action.compare("quits"))
                this->freeAudio();
        } else {
            int nb_sound;
            try {
                nb_sound = stoi(command.substr(12, 13));

                if(nb_sound >= this->sounds.size())
                    return;
            } catch(invalid_argument & e) {
                cout << "Invalid code" << endl;
                return;
            }

            //Initialize the map iterator
            map<string, Sound>::iterator it;
            int index = 0;
            for(it = this->sounds.begin(); it != this->sounds.end() && index != nb_sound; it++)
                index++;

            if(it != this->sounds.end())
                //If comparison result is 0 then pause the current sound.
                if(!action.compare("pause"))
                    it->second.channel->setPaused(true);
                //If comparison result is 0 then resume the current sound.
                else if(!action.compare("resum"))
                    it->second.channel->setPaused(false);
                //If comparison result is 0 then start the current sound or restart it if it's started already and set up volume.
                else if(!action.compare("start")) {
                    //Check if the audio is being played currently, if yes restart it, if not start i.
                    bool playing;

                    this->result = it->second.channel->isPlaying(&playing);
                    
                    /**
                     * Important check if FMOD is working well.
                     * Check for invalid event handlers being exectuted during the program execution.
                     * Check if the system didn't steal the current channel at the request of other processes with higher priority.
                    */
                    if((this->result != FMOD_OK) && (this->result != FMOD_ERR_INVALID_HANDLE) && (this->result != FMOD_ERR_CHANNEL_STOLEN)) {
                        ERRCHECK(this->result);
                    } else if(playing) {
                        it->second.channel->stop();
                    }

                    //Play the sound on the default channel group 0, not paused on sound's channel that he belongs.
                    this->result = this->fmod_system->playSound(it->second.sound, 0, false, &it->second.channel);

                    //Set up desired volume
                    it->second.channel->setVolume(it->second.volume);

                    ERRCHECK(this->result);
                } else return;
        }
    };
}

void Audio::freeAudio() {
    map<string, Sound>::iterator it;

    for(it = this->sounds.begin(); it != this->sounds.end(); it++) {
        this->result = it->second.sound->release();
        ERRCHECK(this->result);
    };

    this->result = this->fmod_system->close();
    ERRCHECK(this->result);

    this->result = this->fmod_system->release();
    ERRCHECK(this->result);

    Common_Close();
}