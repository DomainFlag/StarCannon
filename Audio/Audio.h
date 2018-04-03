#ifndef AUDIO
#define AUDIO

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>
#include <string>

using namespace std;

#include "./src/fmod.hpp"
#include "./other/common.h"

struct Song {
    FMOD::Sound * song;
    FMOD::Channel * channel;
};

class Audio {
public:
	int pipe_fd[2];

	Audio();
	void play(map<string, Song> songs, int * pipe_fd);
	void keyboardListener(GLFWwindow * window, int key, int scancode, int action, int mods);
};


#endif