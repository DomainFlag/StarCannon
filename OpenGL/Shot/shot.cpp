#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "./Shot.h"
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

   Shot shot(mode);

   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   glViewport(0, 0, mode->width, mode->height);

   while(!glfwWindowShouldClose(window)) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;

      shot.renderProgram();

      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   shot.freeProgram();

   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}