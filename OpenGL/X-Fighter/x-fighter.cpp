#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "./X-Fighter.h"
using namespace std;

int main() {
   GLFWwindow * window;

   if(!glfwInit())
      return -1;

   const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

   window = glfwCreateWindow(mode->width, mode->height, "Shot rendering", glfwGetPrimaryMonitor(), NULL);

   if(!window) {
      glfwTerminate();
      return -1;
   }

   glfwMakeContextCurrent(window);

   if(glewInit() != GLEW_OK) {
      cout << "error..!!" << endl;
   }

   cout << glGetString(GL_VERSION) << endl;

   X_Fighter x_fighter(mode);

   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
   glViewport(0, 0, mode->width, mode->height);

   while(!glfwWindowShouldClose(window)) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;

      x_fighter.renderProgram();

      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   x_fighter.freeProgram();

   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}