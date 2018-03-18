#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <assert.h>
#include <unistd.h>
#include <SOIL/SOIL.h>
#include "Sky.h"
#include "./../Tools/Matrix/Matrix.h"
using namespace std;

const GLFWvidmode * mode;

SkyLayer * skyLayer;

void input(GLFWwindow * window, int key, int action, int u, int i) {
	switch(key) {
		case GLFW_KEY_A : {
		 //... strafing translationX neg with rotationZ pos
			break;
		};
		case GLFW_KEY_W : {
			if(skyLayer->translationZ < 1.0f)
				skyLayer->translationZ += 0.05f;
			skyLayer->translation->translation(0.0f, 0.0f, skyLayer->translationZ);
			break;
		};
		case GLFW_KEY_S : {
			if(skyLayer->translationZ > -1.0f)
				skyLayer->translationZ -= 0.05f;
			skyLayer->translation->translation(0.0f, 0.0f, skyLayer->translationZ);
			break;
		};
		case GLFW_KEY_D : {
		 //... strafing translationX pos with rotationZ neg
			break;
		};
	};
}

void cursor(GLFWwindow * window, double x, double y) {
	double variation = 1/50.0;
	skyLayer->offsetY = (x-mode->width/2.0)/(mode->width/2.0)*variation;
	skyLayer->offsetX = (y-mode->height/2.0)/(mode->height/2.0)*variation;
}

int main(int argc, char ** argv) {
	GLFWwindow * window;

	if(!glfwInit())
		return -1;

	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	window = glfwCreateWindow(mode->width, mode->height, "Ocean rendering", glfwGetPrimaryMonitor(), NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	cout << glGetString(GL_VERSION) << endl;

	if(glewInit() != GLEW_OK) {
		std::cout << "error..!!" << std::endl;
	}

	SkyLayer sky;
	skyLayer = &sky;

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glViewport(0, 0, mode->width, mode->height);

	glfwSetKeyCallback(window, input);
	glfwSetCursorPosCallback(window, cursor);

	while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		sky.renderSky();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	sky.freeSky();

	glfwTerminate();
	return 0;
}