#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include "Terrain.h"
using namespace std;

const GLFWvidmode * mode;

Terrain * terrain_p;

void keyboardListener(GLFWwindow * window, int key, int action, int u, int i) {
	switch(key) {
		case GLFW_KEY_ESCAPE : {
			glfwDestroyWindow(window);
			break;
		};
		case GLFW_KEY_A : {
			terrain_p->roll -= 0.05;
			break;
		};
		case GLFW_KEY_W : {
			terrain_p->terrain.speed += terrain_p->terrain.partition/8.0f;
			break;
		};
		case GLFW_KEY_S : {
			terrain_p->terrain.speed -= terrain_p->terrain.partition/8.0f;
			break;
		};
		case GLFW_KEY_D : {
			terrain_p->roll += 0.05;
			break;
		};
	};
}

void cursorListener(GLFWwindow * window, double posX, double posY) {
	/**
	 * Function equation f(x)=1/x will be used to lower the rotation from outer to center of the screen
	 * Where f(x)=1/x has the domain [1/2, 4] with the image [0.25, 2] and [-4, -1/2] with the image [-2, -0.25]
	 * Where, the derivative decreases from 1/2 to 4 and increases from -4 to -1/2
	 */
	float x = ((posX-mode->width/2.0f)/(mode->width/2.0f));
	float y = ((posY-mode->height/2.0f)/(mode->height/2.0f));

	if(x < 0.0f) {
	    x = (x+1.0f)*7.0f/2.0f+1.0f/2.0f;
	    terrain_p->traceYaw = 1.0f/x;
	} else {
	    x = -(x-1.0f)*7.0f/2.0f+1.0f/2.0f;
	    terrain_p->traceYaw = -1.0f/x;
	}

	if(y < 0.0f) {
	    y = (y+1.0f)*7.0f/2.0f+1.0f/2.0f;
	    terrain_p->tracePitch = 1.0f/y;
	} else {
	    y = -(y-1.0f)*7.0f/2.0f+1.0f/2.0f;
	    terrain_p->tracePitch = -1.0f/y;
	}
}

int main(int argc, char ** argv) {
	GLFWwindow * window;

	if(!glfwInit())
		return -1;

	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	window = glfwCreateWindow(mode->width, mode->height, "Terrain rendering", glfwGetPrimaryMonitor(), NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	cout << glGetString(GL_VERSION) << endl;

	if(glewInit() != GLEW_OK) {
		std::cout << "error..!!" << std::endl;
	}

	Terrain terrain(mode);
	terrain_p = &terrain;

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glViewport(0, 0, mode->width, mode->height);

	glfwSetKeyCallback(window, keyboardListener);
	glfwSetCursorPosCallback(window, cursorListener);

	while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		terrain.renderProgram();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	terrain.freeProgram();

	glfwTerminate();
	return 0;
}