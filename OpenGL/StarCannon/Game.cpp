#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include "./../Audio/Audio.h"
#include "./../Tools/Matrix/Matrix.h"
#include "./../Terrain/Terrain.h"
#include "./../SkyBox/Sky.h"
#include "./../Spacecraft/Spacecraft.h"
#include "./../Thruster/Thruster.h"
using namespace std;

const GLFWvidmode * mode;

Audio * audioP;
SkyLayer * skyP;
Thruster * thrusterP;
Spacecraft * spacecraftP;
TerrainLayer * terrainP;

void input(GLFWwindow * window, int key, int action, int u, int i) {
	// terrainP->keyboardListener(window, key, action, u, i);
	// skyP->keyboardListener(window, key, action, u, i);

	switch(key) {
		case GLFW_KEY_ESCAPE : {
			glfwSetWindowShouldClose(window, true);
			break;
		};
	};
};

void cursor(GLFWwindow * window, double x, double y) {
	terrainP->cursorListener(window, x, y);
	skyP->cursorListener(window, x, y);
};

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	string command = "start_sound_0";
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        audioP->changeAudio(command);
    }
}

int main() {
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

	Audio audio;
	audioP = &audio;

	string command = "start_sound_3";
	audioP->changeAudio(command);
	command = "start_sound_2";
	audioP->changeAudio(command);

	// TerrainLayer terrain(mode);
	SkyLayer sky(mode);
	Thruster thruster(mode);
	Spacecraft spacecraft;
	TerrainLayer terrain(mode);

	skyP = &sky;
	thrusterP = &thruster;
	spacecraftP = &spacecraft;
	terrainP = &terrain;

	glfwSetKeyCallback(window, input);
	glfwSetCursorPosCallback(window, cursor);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glViewport(0, 0, mode->width, mode->height);

	glfwSetKeyCallback(window, input);
	glfwSetCursorPosCallback(window, cursor);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		sky.setParameters();
		sky.renderProgram();

		spacecraft.setParameters();
		spacecraft.renderProgram();

		terrain.setParameters();
		terrain.renderProgram();

		thruster.setParameters();
		thruster.renderProgram();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	terrain.freeProgram();
	sky.freeProgram();
	terrain.freeProgram();
	thruster.freeProgram();
	audio.freeAudio();

	glfwTerminate();
	return 0;
}