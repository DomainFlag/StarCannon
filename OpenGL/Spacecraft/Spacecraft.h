#ifndef SPACECRAFT
#define SPACECRAFT

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include "./../Tools/Matrix/Matrix.h"
using namespace std;

class Spacecraft {
public:
   vector<float> cube;
   vector<float> palette;

   unsigned int program;

   Matrix viewMatrix, cameraMatrix, perspectiveMatrix;
   Matrix rotationX;

   float rotX = 0.0f;

   GLint attribPositionLoc, attribColorLoc; 
   GLint unifCameraLoc, unifPerspectiveLoc;

   GLuint positionBuffer, colorBuffer; 

   string vertexShader = R"(
      #version 130
      attribute vec4 a_color;
      attribute vec4 a_position;

      uniform mat4 u_camera;
      uniform mat4 u_perspective;

      varying vec4 v_color;

      void main() {
        vec4 pos = u_camera*a_position;
        v_color = a_color;
        gl_Position = pos;
      }
   )";

   string fragmentShader = R"(
      #version 130
      precision mediump float;
      varying vec4 v_color;

      void main() {
        gl_FragColor = vec4(v_color.rgb, 1.0);
      }
   )";

   Spacecraft();

   vector<float> setCube(float x1, float y1, float length);

   void setColors();

   void listenContinouslyToCursor();

   void act();

   void setParameters();

   void setVariablesLocation();

   void setVariablesData();

   void renderProgram();

   void freeProgram();
};

#endif