#ifndef SKY_BOX
#define SKY_BOX

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <SOIL/SOIL.h>
#include "./../Tools/Matrix/Matrix.h"
using namespace std;

class SkyLayer {
public:
   const GLFWvidmode * mode;

   Matrix cameraMatrix;
   Matrix quaternionMatrix, cameraRotX, cameraRotY, cameraRotZ;

   float yaw = 0, pitch = 0, roll = 0;
   float traceYaw = 0,tracePitch = 0, traceRoll = 0;

   unsigned int program;
   unsigned int bufferPosition;

   vector<float> pos = {
      -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
      1.0f, 1.0f, -1.0f, -1.0f,  -1.0f, -1.0f, 1.0f, -1.0f, -1.0f
   };

   GLint attribPositionLoc, unifCameraLoc;

   string vertexSkyShader = R"(
      #version 130
      attribute vec4 a_position;
      uniform mat4 u_camera;
      varying vec4 v_texture;

      void main() {
         gl_Position = vec4(a_position.xyz, 1.0);
         v_texture = u_camera*a_position;
      }
   )";

   string fragmentSkyShader = R"(
      #version 130
      precision mediump float;
      varying vec4 v_texture;
      uniform samplerCube u_skybox;

      void main() {
         gl_FragColor = textureCube(u_skybox, v_texture.xyz);
      } 
   )";

   SkyLayer(const GLFWvidmode * mode);

   void cursorListener(GLFWwindow * window, double posX, double posY);

   void keyboardListener(GLFWwindow * window, int key, int scancode, int action, int mods);

   void listenContinouslyToCursor();

   void setParameters();

   void setVariablesLocation();

   void setVariablesData();

   void renderProgram();

   void freeProgram();
};

#endif