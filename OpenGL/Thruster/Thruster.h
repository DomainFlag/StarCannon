#ifndef THRUSTER
#define THRUSTER

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "./../Tools/Matrix/Matrix.h"
using namespace std;

class Thruster {
public:
   unsigned int program;
   const GLFWvidmode * mode;

   GLint attribPositionLoc, unifCameraLoc, unifPerspectiveLoc; 
   unsigned int positionBuffer;

   int nbParticles = 30000;
   float tail = -1.0f;
   float bellfat = 30.0;
   float speed = 0.01f;

   Matrix viewMatrix, cameraMatrix, perspectiveMatrix;
   Matrix cameraTranslation, cameraRotX, cameraRotY, cameraRotZ;
   float translationY = 0.0f, translationZ = 0.0f;
   float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;

   vector<float> particles;

   string vertexShader = R"(
      #version 130

      attribute vec4 a_position;
      varying float v_color;
      uniform mat4 u_camera;
      uniform mat4 u_perspective;
      void main() {
         vec4 pos = u_perspective*u_camera*a_position;
         gl_PointSize = 3.5;
         gl_Position = pos;
         v_color = a_position.z+1.0; //->[0, 1]
      }
   )";

   string fragmentShader = R"(
      #version 130

      precision mediump float;
      varying float v_color;
      void main() {
        gl_FragColor = vec4(0.811, 0.227, 0.141, 1.0-v_color); //->[0, 1]
      }
   )";

   Thruster(const GLFWvidmode * mode);

   void initThruster();

   float bell(float x);

   void act();

   void listenContinouslyToCursor();

   void setParameters();

   void setVariablesLocation();

   void setVariablesData();

   void renderProgram();

   void freeProgram();
};

#endif