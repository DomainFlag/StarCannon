#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "./Thruster.h"
using namespace std;

Thruster * thruster_p;

void keyboardListener(GLFWwindow * window, int key, int action, int u, int i) {
   if(GLFW_KEY_W == key) {
      thruster_p->speed += thruster_p->stepSpeed;
      thruster_p->errorX = thruster_p->lerp();
   } else if(GLFW_KEY_S == key) {
      thruster_p->speed -= thruster_p->stepSpeed;
      thruster_p->errorX = thruster_p->lerp();
   }

   if(GLFW_KEY_E == key) {
      thruster_p->rotation[2] += 0.02f;
   } else if(GLFW_KEY_D == key) {
      thruster_p->rotation[2] -= 0.02f;
   }

   if(GLFW_KEY_R == key) {
      thruster_p->rotation[1] += 0.02f;
   } else if(GLFW_KEY_F == key) {
      thruster_p->rotation[1] -= 0.02f;
   }

   if(GLFW_KEY_UP == key) {
      thruster_p->transl[1] += 0.001f;
   } else if(GLFW_KEY_DOWN == key) {
      thruster_p->transl[1] -= 0.001f;
   }

   if(GLFW_KEY_LEFT == key) {
      thruster_p->transl[0] -= 0.001f;
   } else if(GLFW_KEY_RIGHT == key) {
      thruster_p->transl[0] += 0.001f;
   }

   if(GLFW_KEY_ESCAPE == key)
      glfwSetWindowShouldClose(window, true);
}

void cursorListener(GLFWwindow * window, double x, double y) {}

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

   Thruster thruster(mode);
   thruster_p = &thruster;

   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   glViewport(0, 0, mode->width, mode->height);

   glfwSetKeyCallback(window, keyboardListener);

   while(!glfwWindowShouldClose(window)) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;

      thruster.renderProgram();

      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   thruster.freeProgram();

   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}