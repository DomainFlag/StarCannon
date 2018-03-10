#include "./src/fmod.hpp"
#include "./other/common.h"

#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <iostream>
#include <sstream>
#include <map>
using namespace std;

struct Song {
    FMOD::Sound * song;
    FMOD::Channel * channel;
};

void playAudio(map<string, Song> songs, int * pipe_fd) {
    FMOD::System     *system;
    FMOD_RESULT       result;
    unsigned int      version;
    void             *extradriverdata = 0;

    /* Create a System object and initialize */
    result = FMOD::System_Create(&system);
    ERRCHECK(result);

    result = system->getVersion(&version);
    ERRCHECK(result);

    if(version < FMOD_VERSION) {
        Common_Fatal("FMOD lib version %08x doesn't match header version %08x", version, FMOD_VERSION);
    }

    result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
    ERRCHECK(result);

    map<string, Song>::iterator it;
    for(it = songs.begin(); it != songs.end(); it++) {
        result = system->createSound(Common_MediaPath(it->first.c_str()), FMOD_DEFAULT, 0, &it->second.song);
        ERRCHECK(result);
    };

    // Not important for now...
    // result = sound1->setMode(FMOD_LOOP_OFF);    /* kick.wav has embedded loop points which automatically makes looping turn on, */
    // ERRCHECK(result);                           /* so turn it off here.  We could have also just put FMOD_LOOP_OFF in the above CreateSound call. */

    char key[20];
    int chunk;
    bool playing = 0;
    while(true) {
        chunk = 0;
        while(chunk < 20) {
            int dataRead = read(pipe_fd[0], key+chunk, 1);
            chunk += dataRead;
            if(key[chunk-1] == '\0')
                break;
        }

        if(chunk > 0) {
            map<string, Song>::iterator it;
            int counter = 0;
            string string_key(key);

            string action = string_key.substr(0, 5);
            int nb_song = stoi(string_key.substr(11, 12));

            for(it = songs.begin(); it != songs.end(); it++) {
                string currentKey = it->first;

                if(counter < nb_song) {
                    counter++;
                    continue;
                }

                if(!action.compare("pause")) {
                    it->second.channel->setPaused(true);
                } else if(!action.compare("resum")) {
                    it->second.channel->setPaused(false);
                } else if(!action.compare("start")) {
                    result = it->second.channel->isPlaying(&playing);
                    if((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
                        ERRCHECK(result);
                    } else if(playing) {
                        it->second.channel->stop();
                    }
                    result = system->playSound(it->second.song, 0, false, &it->second.channel);
                    ERRCHECK(result);
                }

                break;
            }

            // if(strcmp(key, "songp0") == 0)
            //     songs["kick.wav"]->setPaused(true);
            // else if(strcmp(key, "songp1") == 0)
            //     songs["obstacle1.mp3"]->setPaused(true); 
            // else if(strcmp(key, "songp2") == 0)
            //     songs["pjano.wav"]->setPaused(true); 
            // else if(strcmp(key, "songr0") == 0)
            //     songs["kick.wav"]->setPaused(false);
            // else if(strcmp(key, "songr1") == 0)
            //     songs["obstacle1.mp3"]->setPaused(false);
            // else if(strcmp(key, "songr2") == 0)
            //     songs["pjano.wav"]->setPaused(false);
            // else if(strcmp(key, "songs0") == 0) {
            //     result = system->playSound(sound1, 0, false, &songs["kick.wav"]);
            //     ERRCHECK(result);
            // } else if(strcmp(key, "songs1") == 0) {
            //     result = songs["obstacle1.mp3"]->isPlaying(&playing);
            //     if((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
            //         ERRCHECK(result);
            //     } else if(playing) {
            //         songs["obstacle1.mp3"]->stop();
            //     }
            //     result = system->playSound(sound2, 0, false, &songs["obstacle1.mp3"]);
            //     ERRCHECK(result);
            // } else if(strcmp(key, "songs2") == 0) {
            //     result = system->playSound(sound3, 0, false, &songs["pjano.wav"]);
            //     ERRCHECK(result);
            // };
        }
    }


    for(it = songs.begin(); it != songs.end(); it++) {
        result = it->second.song->release();
        ERRCHECK(result);
    };
    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    Common_Close();
}

int main() {
    map<string, Song> songs;
    songs["kick.wav"] = Song{0, NULL};
    songs["obstacle1.mp3"] = Song{0, NULL};
    songs["seven_nation_army.mp3"] = Song{0, NULL};

    int songsLength = songs.size()-1;
    stringstream s;
    s << "StarCannon DJ powered by FMOD \n ";
    s << "Total songs: " << songsLength << " \n ";
    s << "Key-codes to write in the console for sound management: \n";
    s << "start_song_[0-" << songsLength << "] - starts/resets a song from 0 to " << songsLength << " \n";
    s << "pause_song_[0-" << songsLength << "] - pauses a song from 0 to " << songsLength << " \n";
    s << "resum_song_[0-" << songsLength << "] - resumes a song from 0 to " << songsLength << " \n";
    cout << s.str() << endl;

    int pipe_fd[2];
    pipe(pipe_fd);

    pid_t pid = fork();

    if(pid < 0) {
        exit(1);
    } else if(pid == 0) {
        playAudio(songs, pipe_fd);
    } else {
        char key[20];
        int chunk;
        while(true) {
            cin >> key;
            chunk = 0;
            while(chunk < 20) {
                int dataWritten = write(pipe_fd[1], key+chunk, 1);
                chunk += dataWritten;
                if(key[chunk-1] == '\0')
                    break;
            }
        }
    }

    // do
    // {
    //     Common_Update();

    //     if (Common_BtnPress(BTN_ACTION1))
    //     {
    //         result = system->playSound(sound1, 0, false, &channel);
    //         ERRCHECK(result);
    //     }

    //     if (Common_BtnPress(BTN_ACTION2))
    //     {
    //         result = system->playSound(sound2, 0, false, &channel);
    //         ERRCHECK(result);
    //     }

    //     if (Common_BtnPress(BTN_ACTION3))
    //     {
    //         result = system->playSound(sound3, 0, false, &channel);
    //         ERRCHECK(result);
    //     }

    //     result = system->update();
    //     ERRCHECK(result);

    //     {
    //         unsigned int ms = 0;
    //         unsigned int lenms = 0;
    //         bool         playing = 0;
    //         bool         paused = 0;
    //         int          channelsplaying = 0;

    //         if (channel)
    //         {
    //             FMOD::Sound *currentsound = 0;

    //             result = channel->isPlaying(&playing);
    //             if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
    //             {
    //                 ERRCHECK(result);
    //             }

    //             result = channel->getPaused(&paused);
    //             if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
    //             {
    //                 ERRCHECK(result);
    //             }

    //             result = channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
    //             if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
    //             {
    //                 ERRCHECK(result);
    //             }
               
    //             channel->getCurrentSound(&currentsound);
    //             if (currentsound)
    //             {
    //                 result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
    //                 if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
    //                 {
    //                     ERRCHECK(result);
    //                 }
    //             }
    //         }

    //         system->getChannelsPlaying(&channelsplaying, NULL);
    //     }

    //     Common_Sleep(50);
    // } while (!Common_BtnPress(BTN_QUIT));

    /*
        Shut down
    */
    // result = sound1->release();
    // ERRCHECK(result);
    // result = sound2->release();
    // ERRCHECK(result);
    // result = sound3->release();
    // ERRCHECK(result);
    // result = system->close();
    // ERRCHECK(result);
    // result = system->release();
    // ERRCHECK(result);

    // Common_Close();

    return 0;
}
