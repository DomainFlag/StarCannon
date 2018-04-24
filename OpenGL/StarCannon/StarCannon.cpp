#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <SOIL/SOIL.h>
#include "./../Shader/Shader.h"
#include "./../Tools/ObjReader/OBJReader.h"
#include "./../Tools/Matrix/Matrix.h"
#include "./../X-Fighter/X-Fighter.h"
#include "./../Audio/Audio.h"
#include "./../Thruster/Thruster.h"
#include "./../Blaster/Blaster.h"
using namespace std;

const GLFWvidmode * mode;

Blaster * blaster_p;
Audio * audio_p;

string song = "start_sound_3";
string flight = "start_sound_2";
string shot = "start_sound_0";

void input(GLFWwindow * window, int key, int action, int u, int i) {
	// terrainP->keyboardListener(window, key, action, u, i);
	// skyP->keyboardListener(window, key, action, u, i);

	// audioP->keyboardListener(window, key, action, u, i);

	switch(key) {
		case GLFW_KEY_ESCAPE : {
			glfwSetWindowShouldClose(window, true);
			break;
		};
	};
};

void cursor(GLFWwindow * window, double x, double y) {
	// terrainP->cursorListener(window, x, y);
	// skyP->cursorListener(window, x, y);
};

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        blaster_p->fireShots();

        audio_p->changeAudio(shot);
    }
}

int main(int argc, char ** argv) {
	GLFWwindow * window;

	if(!glfwInit())
		return -1;

	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	window = glfwCreateWindow(mode->width, mode->height, "StarCannon", glfwGetPrimaryMonitor(), NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	cout << glGetString(GL_VERSION) << endl;

	if(glewInit() != GLEW_OK) {
		std::cout << "error..!!" << std::endl;
	}

	X_Fighter x_fighter(mode);
	Thruster thruster(mode);
	Blaster blaster(mode);
	Audio audio;

	audio.changeAudio(song);
	audio.changeAudio(flight);

	blaster_p = &blaster;
	audio_p = &audio;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, mode->width, mode->height);

	glfwSetKeyCallback(window, input);
	// glfwSetCursorPosCallback(window, cursor);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		blaster.renderShots();

		x_fighter.renderProgram();

		thruster.renderProgram();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	x_fighter.freeProgram();

	thruster.freeProgram();

	glfwTerminate();
	return 0;
}