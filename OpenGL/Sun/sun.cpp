#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include "Sun.h"
using namespace std;

const GLFWvidmode * mode;

Sun * sun_p;

int main(int argc, char ** argv) {
	GLFWwindow * window;

	if(!glfwInit())
		return -1;

	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	window = glfwCreateWindow(mode->width, mode->height, "Sun rendering", glfwGetPrimaryMonitor(), NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	cout << glGetString(GL_VERSION) << endl;

	if(glewInit() != GLEW_OK) {
		std::cout << "error..!!" << std::endl;
	}

	Sun sun(mode);
	sun_p = &sun;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, mode->width, mode->height);

	while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		sun.renderProgram();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	sun.freeProgram();

	glfwTerminate();
	return 0;
}