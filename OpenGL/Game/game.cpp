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
#include "./../Tools/Matrix/Matrix.h"
#include "./../Ocean/Ocean.h"
#include "./../Sky/Sky.h"
using namespace std;

const GLFWvidmode * mode;

OceanLayer * oceanLayer;
SkyLayer * skyLayer;

void input(GLFWwindow * window, int key, int action, int u, int i) {
	switch(key) {
		case GLFW_KEY_Q : {
			oceanLayer->rotX += 0.05;
			oceanLayer->rotationX->rotationX(oceanLayer->rotX);
			break;
		};
		case GLFW_KEY_W : {
			oceanLayer->rotY += 0.05;
			oceanLayer->rotationY->rotationY(oceanLayer->rotY);
			break;
		};
		case GLFW_KEY_A : {
			oceanLayer->rotX -= 0.05;
			oceanLayer->rotationX->rotationX(oceanLayer->rotX);
			break;
		};
		case GLFW_KEY_S : {
			oceanLayer->rotY -= 0.05;
			oceanLayer->rotationY->rotationY(oceanLayer->rotY);
			break;
		};
		case GLFW_KEY_E : {
			oceanLayer->rotZ += 0.05;
			oceanLayer->rotationZ->rotationZ(oceanLayer->rotZ);
			break;
		};
		case GLFW_KEY_D : {
			oceanLayer->rotZ -= 0.05;
			oceanLayer->rotationZ->rotationZ(oceanLayer->rotZ);
			break;
		};

		case GLFW_KEY_R : {
			oceanLayer->distance += 0.05;
			oceanLayer->translationXYZ->translation(0.0f, oceanLayer->distance, 0.0f);
			break;
		};
		case GLFW_KEY_F : {
			oceanLayer->distance -= 0.05;
			oceanLayer->translationXYZ->translation(0.0f, oceanLayer->distance, 0.0f);
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

	window = glfwCreateWindow(mode->width, mode->height, "StarCannon rendering", glfwGetPrimaryMonitor(), NULL);
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
	SkyLayer sky;

	skyLayer = &sky;
	oceanLayer = &ocean;

	glfwSetKeyCallback(window, input);
	glfwSetCursorPosCallback(window, cursor);

	ocean.setSettings();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glViewport(0, 0, mode->width, mode->height);

	glfwSetKeyCallback(window, input);
	glfwSetCursorPosCallback(window, cursor);

	while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindBuffer(GL_ARRAY_BUFFER, sky.bufferPosition);
		glVertexAttribPointer(sky.vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		sky.renderSky();

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		glBindBuffer(GL_ARRAY_BUFFER, ocean.positionBuffer);
		glVertexAttribPointer(ocean.vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		ocean.renderOcean();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ocean.freeOcean();
	sky.freeSky();

	glfwTerminate();
	return 0;
}