#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "./Blaster.h"
using namespace std;

Blaster * blaster_p;

void input(GLFWwindow * window, int key, int action, int u, int i) {

	switch(key) {
		case GLFW_KEY_ESCAPE : {
			glfwSetWindowShouldClose(window, true);
			break;
		};
	};
};

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        blaster_p->fireShots();
    }
}

int main() {
   GLFWwindow * window;

   if(!glfwInit())
      return -1;

   const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

   window = glfwCreateWindow(mode->width, mode->height, "Thruster rendering", glfwGetPrimaryMonitor(), NULL);

   if(!window) {
      glfwTerminate();
      return -1;
   }

   glfwMakeContextCurrent(window);

   if(glewInit() != GLEW_OK) {
      cout << "error..!!" << endl;
   }

   cout << glGetString(GL_VERSION) << endl;

   Blaster blaster(mode);
   blaster_p = &blaster;

   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   glViewport(0, 0, mode->width, mode->height);
   
   glfwSetKeyCallback(window, input);
   glfwSetMouseButtonCallback(window, mouse_button_callback);

   while(!glfwWindowShouldClose(window)) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;

      blaster.renderShots();

      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}