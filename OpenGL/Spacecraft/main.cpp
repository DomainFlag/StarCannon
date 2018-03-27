#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "Spacecraft.h"
using namespace std;

Spacecraft * spacecraftP;

const GLFWvidmode * mode;

void keyboardListener(GLFWwindow * window, int key, int action, int u, int i) {
   if(GLFW_KEY_W == key) {
      spacecraftP->rotX += 0.01f;   
   } else if(GLFW_KEY_S == key) {
      spacecraftP->rotX -= 0.01f; 
   }

   // if(GLFW_KEY_UP == key) {
   //    thrusterP->translationY += 0.05f;
   // } else if(GLFW_KEY_DOWN == key) {
   //    thrusterP->translationY -= 0.05f; 
   // }

   // if(GLFW_KEY_LEFT == key) {
   //    thrusterP->rotX += 0.05f;
   //    thrusterP->cameraRotX.rotationX(thrusterP->rotX);
   // } else if(GLFW_KEY_RIGHT == key) {
   //    thrusterP->rotX -= 0.05f; 
   //    thrusterP->cameraRotX.rotationX(thrusterP->rotX);
   // // }
}

void cursorListener(GLFWwindow * window, double x, double y) {}

int main(int argc, char ** argv) {
   GLFWwindow * window;

   if(!glfwInit())
      return -1;

   mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

   window = glfwCreateWindow(mode->width, mode->height, "Spacecraft rendering", glfwGetPrimaryMonitor(), NULL);
   if(!window) {
      glfwTerminate();
      return -1;
   }

   glfwMakeContextCurrent(window);

   if(glewInit() != GLEW_OK) {
      cout << "error..!!" << endl;
   }

   cout << glGetString(GL_VERSION) << endl;

   Spacecraft spacecraft;
   spacecraftP = &spacecraft;

   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
   glViewport(0, 0, mode->width, mode->height);

   glfwSetKeyCallback(window, keyboardListener);
   glfwSetCursorPosCallback(window, cursorListener);

   while(!glfwWindowShouldClose(window)) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      spacecraft.renderProgram();

      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   spacecraft.freeProgram();

   glfwTerminate();
   return 0;
}