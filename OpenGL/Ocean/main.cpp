#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <assert.h>
#include <unistd.h>
#include <SOIL/SOIL.h>
#include "Ocean.h"
using namespace std;

const GLFWvidmode * mode;

OceanLayer * oceanLayer;

void input(GLFWwindow * window, int key, int action, int u, int i) {
	switch(key) {
		case GLFW_KEY_Q : {
			oceanLayer->rotX += 0.05;
			oceanLayer->rotationX.rotationX(oceanLayer->rotX);
			break;
		};
		case GLFW_KEY_W : {
			oceanLayer->rotY += 0.05;
			oceanLayer->rotationY.rotationY(oceanLayer->rotY);
			break;
		};
		case GLFW_KEY_A : {
			oceanLayer->rotX -= 0.05;
			oceanLayer->rotationX.rotationX(oceanLayer->rotX);
			break;
		};
		case GLFW_KEY_S : {
			oceanLayer->rotY -= 0.05;
			oceanLayer->rotationY.rotationY(oceanLayer->rotY);
			break;
		};
		case GLFW_KEY_E : {
			oceanLayer->rotZ += 0.05;
			oceanLayer->rotationZ.rotationZ(oceanLayer->rotZ);
			break;
		};
		case GLFW_KEY_D : {
			oceanLayer->rotZ -= 0.05;
			oceanLayer->rotationZ.rotationZ(oceanLayer->rotZ);
			break;
		};
	};
}

static void cursor(GLFWwindow * window, double x, double y) {
	// double unitX = (2*x-mode->width)/mode->width;
	// double unitY = (2*y-mode->height)/mode->height;

	// float translationX = ((float) unitX)/5.0;
	// float translationY = -((float) unitY)/5.0;

	// delete translation;
	// translation = new Matrix();
	// translation->translation(translationX, translationY, 0.0f);
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

	OceanLayer ocean(mode);
	oceanLayer = &ocean;

	ocean.setSettings();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glViewport(0, 0, mode->width, mode->height);

	glfwSetKeyCallback(window, input);
	glfwSetCursorPosCallback(window, cursor);

	while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ocean.renderOcean();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ocean.freeOcean();

	glfwTerminate();
	return 0;
}