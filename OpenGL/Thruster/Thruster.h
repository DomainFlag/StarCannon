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
   GLuint program;

   const GLFWvidmode * mode;

   float maxSpeed = 0.05;
   float minSpeed = 0.01;
   float stepSpeed = 0.0005;
   float speed = this->minSpeed;

   float tail = 0.6;
   vector<float> color{114, 236, 254};
   float errorX = 0.01;
   float errorY = 0.005;
   float amplitude = 0.4;

   float nbParticles = 60000;
   float range = 0.8;

   vector<float> particles;
   vector<float> translations;

   GLint attribPosLoc, attribTranslLoc;
   GLint unifModelViewLoc, uniProjectionLoc, unifCameraLoc; 
   GLint unifColorLoc, unifTailLoc, unifScalarTailNormalizerLoc;

   GLuint posBuffer, translBuffer;

   vector<float> position{0, -1.175, 0};
   vector<float> transl{0, 0, -0.5};
   vector<float> rotation{0, -60, -90};

   Matrix modelView, lookAt, projection;
   Matrix viewMatrix, translation;

   string vertexShader = R"(
      #version 130

      attribute vec4 a_position;
      attribute vec4 a_translation;
      
      uniform mat4 u_model;
      uniform mat4 u_camera;
      uniform mat4 u_projection;

      uniform float u_tail;
      uniform float u_scalarTailNormalizer;
      
      varying float v_far;
      varying float v_dispersion;
      
      void main() {
          gl_PointSize = 1.5;

          vec4 rotated_pos = u_model*vec4(a_position.xyz, 1.0);
          
          gl_Position = u_projection*u_camera*(a_translation + rotated_pos);
          
          float len = length(vec3(0, a_position.yz))/a_position.w;
          
          v_far = 1.0/exp(-2.0*(a_position.x+u_tail/2.0))/u_scalarTailNormalizer;
          v_dispersion = len/5.0;
      }
   )";

   string fragmentShader = R"(
      #version 130

      precision mediump float;
      
      uniform vec3 u_color;
      
      varying float v_far;
      varying float v_dispersion;
      
      //RGB to HSV convert
      vec3 rgb2hsv(vec3 c) {
          vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
          vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
          vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
      
          float d = q.x - min(q.w, q.y);
          float e = 1.0e-10;
          return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
      }
       
      //HSV to RGB convert
      vec3 hsv2rgb(vec3 c) {
          vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
          vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
          return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
      }
      
      void main() {
          vec3 hsv = rgb2hsv(u_color);
          hsv.r += v_dispersion;
          hsv.b -= v_far;
          hsv.b -= v_dispersion;
          vec3 color = hsv2rgb(hsv);
          
          gl_FragColor = vec4(color, 1.0-v_far);
      }
   )";

   Thruster(const GLFWvidmode * mode);

   vector<float> projectParticleProperties(float x);

   vector<float> initializeParticle();

   void initializeThruster();

   void normalizeColor();

   float lerp();

   vector<float> recycle();

   float bell(float x);

   float getMaxBell(float x);

   void act();

   void keyboardListener(GLFWwindow * window, int key, int action, int u, int i);

   void setParameters();

   void setVariablesLocation();

   void setVariablesData();

   void renderProgram();

   void freeProgram();
};

#endif