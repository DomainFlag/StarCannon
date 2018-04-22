#include "./src/fmod.hpp"
#include "./other/common.h"

#include <unistd.h>
#include <iostream>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
using namespace std;

struct Sound {
    FMOD::Sound * sound;

    //Unique channel on what the sound will be played
    FMOD::Channel * channel;

    //Sound volume [0.0, 1.0]
    float volume;
};

//The FMOD system used for audio playback
FMOD::System  * fmod_system;

//The FMOD result var used for debugging errors within the FMOD system
FMOD_RESULT     result;

//FMOD version used for audio playback
unsigned int    version;

//By default 0 to avoid unnecessary specific driver specific data, by standard wav, mp3...
void          * extradriverdata = 0;

void initAudio(map<string, Sound> & sounds) {

    /* Create a System object and initialize */
    result = FMOD::System_Create(&fmod_system);
    ERRCHECK(result);

    result = fmod_system->getVersion(&version);
    ERRCHECK(result);

    if(version < FMOD_VERSION) {
        Common_Fatal("FMOD lib version doesn't match header version ", version, FMOD_VERSION);
    }


    //Allocate sounds.size() channels for the full possibility that every sound can be played in the same time
    result = fmod_system->init(sounds.size(), FMOD_INIT_NORMAL, extradriverdata);
    ERRCHECK(result);

    map<string, Sound>::iterator it;
    for(it = sounds.begin(); it != sounds.end(); it++) {
        result = fmod_system->createSound(Common_MediaPath(it->first.c_str()), FMOD_DEFAULT, 0, &it->second.sound);
        ERRCHECK(result);
    };

    //Setting up LOOP turned on for the rocket_engine
    result = sounds["rocket_engine.wav"].sound->setMode(FMOD_LOOP_NORMAL);
    ERRCHECK(result);
}

void changeAudio(map<string, Sound> & sounds, string & command, bool & eventLoop) {
    if(command.length() >= 5) {
        string action = command.substr(0, 5);

        if(command.length() != 13) {
            if(!action.compare("quits"))
                eventLoop = false;
        } else {
            int nb_sound;
            try {
                nb_sound = stoi(command.substr(12, 13));

                if(nb_sound >= sounds.size())
                    return;
            } catch(invalid_argument & e) {
                cout << "Invalid code" << endl;
                return;
            }

            //Initialize the map iterator
            map<string, Sound>::iterator it;
            int index = 0;
            for(it = sounds.begin(); it != sounds.end() && index != nb_sound; it++)
                index++;

            if(it != sounds.end())
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

                    result = it->second.channel->isPlaying(&playing);
                    
                    /**
                     * Important check if FMOD is working well.
                     * Check for invalid event handlers being exectuted during the program execution.
                     * Check if the system didn't steal the current channel at the request of other processes with higher priority.
                    */
                    if((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
                        ERRCHECK(result);
                    } else if(playing) {
                        it->second.channel->stop();
                    }

                    //Play the sound on the default channel group 0, not paused on sound's channel that he belongs.
                    result = fmod_system->playSound(it->second.sound, 0, false, &it->second.channel);

                    //Set up desired volume
                    it->second.channel->setVolume(it->second.volume);

                    ERRCHECK(result);
                } else return;
        }
    };
}


void freeAudio(map<string, Sound> & sounds) {
    map<string, Sound>::iterator it;

    for(it = sounds.begin(); it != sounds.end(); it++) {
        result = it->second.sound->release();
        ERRCHECK(result);
    };

    result = fmod_system->close();
    ERRCHECK(result);

    result = fmod_system->release();
    ERRCHECK(result);

    Common_Close();
}


int main() {
    map<string, Sound> sounds;
    sounds["obstacle1.mp3"] = Sound{0, NULL, 1.0};
    sounds["white_stripes.mp3"] = Sound{0, NULL, 1.0};
    sounds["blaster_shot.wav"] = Sound{0, NULL, 0.5};
    sounds["rocket_engine.wav"] = Sound{0, NULL, 0.25};

    int soundsLength = sounds.size()-1;
    stringstream s;
    s << "StarCannon DJ powered by FMOD \n ";
    s << "Total sounds: " << soundsLength << " \n ";
    s << "Key-codes to write in the console for sound management: \n";
    s << "start_sound_[0-" << soundsLength << "] - starts/resets a sound from 0 to " << soundsLength << " \n";
    s << "pause_sound_[0-" << soundsLength << "] - pauses a sound from 0 to " << soundsLength << " \n";
    s << "resum_sound_[0-" << soundsLength << "] - resumes a sound from 0 to " << soundsLength << " \n";
    cout << s.str() << endl;

    initAudio(sounds);

    bool eventLoop = true;
    string command = "start_sound_3";
    changeAudio(sounds, command, eventLoop);
    sleep(4);
    command = "start_sound_2";
    changeAudio(sounds, command, eventLoop);
    sleep(4);
    command = "pause_sound_3";
    changeAudio(sounds, command, eventLoop);
    sleep(4);
    command = "resum_sound_3";
    changeAudio(sounds, command, eventLoop);
    sleep(4);
    command = "start_sound_3";
    changeAudio(sounds, command, eventLoop);
    sleep(4);
    command = "quits";
    changeAudio(sounds, command, eventLoop);

    freeAudio(sounds);

    return 0;
}
