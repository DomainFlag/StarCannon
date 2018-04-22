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

   if(GLFW_KEY_UP == key) {
      thruster_p->rotationZ += 0.02f;
      thruster_p->rotZ.rotationX(thruster_p->rotationX);
   } else if(GLFW_KEY_DOWN == key) {
      thruster_p->rotationZ -= 0.02f;
      thruster_p->rotZ.rotationX(thruster_p->rotationX);
   }

   if(GLFW_KEY_LEFT == key) {
      thruster_p->rotationY += 0.02f;
      thruster_p->rotY.rotationY(thruster_p->rotationY);
   } else if(GLFW_KEY_RIGHT == key) {
      thruster_p->rotationY -= 0.02f;
      thruster_p->rotY.rotationY(thruster_p->rotationY);
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

   Thruster thruster;
   thruster_p = &thruster;

   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   glViewport(0, 0, mode->width, mode->height);

   glfwSetKeyCallback(window, keyboardListener);
   // glfwSetCursorPosCallback(window, cursorListener);

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