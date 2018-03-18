#ifndef SKYLAYER
#define SKYLAYER

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <assert.h>
#include <unistd.h>
#include <SOIL/SOIL.h>
#include "./../Tools/Matrix/Matrix.h"
using namespace std;

class SkyLayer {
public:
   Matrix * rotationMX = new Matrix();
   Matrix * rotationMY = new Matrix();
   Matrix * rotationMZ = new Matrix();
   float rotationX = 0.0f;
   float rotationY = 0.0f;
   float rotationZ = 0.0f;

   Matrix * translation = new Matrix();
   float translationZ = 0.0f;

   double offsetX = 0.0f;
   double offsetY = 0.0f;

   unsigned int program;
   unsigned int bufferPosition;

   Matrix * u_swap1 = new Matrix();
   Matrix * u_swap2 = new Matrix();

   GLint vertexPositionLocation, matrixCameraLocation;

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

   SkyLayer();

   void setSettings();

   void setDataLocations();

   void setData();

   void renderSky();

   void freeSky();
};

#endif