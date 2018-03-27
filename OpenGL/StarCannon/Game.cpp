#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include "./../Tools/Matrix/Matrix.h"
#include "./../Terrain/Terrain.h"
#include "./../Sky/Sky.h"
#include "./../Spacecraft/Spacecraft.h"
#include "./../Thruster/Thruster.h"
using namespace std;

const GLFWvidmode * mode;

SkyLayer * skyP;
Thruster * thrusterP;
Spacecraft * spacecraftP;
TerrainLayer * terrainP;

void input(GLFWwindow * window, int key, int action, int u, int i) {
	terrainP->keyboardListener(window, key, action, u, i);
	skyP->keyboardListener(window, key, action, u, i);
};

void cursor(GLFWwindow * window, double x, double y) {
	terrainP->cursorListener(window, x, y);
	skyP->cursorListener(window, x, y);
};

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

	glfwTerminate();
	return 0;
}