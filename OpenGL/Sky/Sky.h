#ifndef SKY
#define SKY

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include "./../Tools/Matrix/Matrix.h"
using namespace std;

class Sky {
public:
    const GLFWvidmode * mode;

    GLuint program;

    vector<float> mesh {
      -1, -1, -1,
      -1, 1, -1,
      1, 1, -1,
      1, 1, -1,
      1, -1, -1,
      -1, -1, -1
    };

    vector<float> color{225, 152, 190};
    float past = 0;
    float rotX = 0, rotY = 0, rotZ = 0;

    Matrix viewMatrix;
    Matrix rotationX, rotationY, rotationZ;

    GLuint posSkyBuffer;

    GLint attribPosSkyLoc;
    GLint unifPitchLoc, unifYawLoc, unifColorSkyLoc, unifSunSkyLoc;

    string vertexShader = R"(
      #version 130
      attribute vec4 a_skyPos;
      
      varying vec3 v_skyLoc;
      
      void main() {
          gl_Position = a_skyPos;
          
          v_skyLoc = a_skyPos.xyz;
      }
    )";

    string fragmentShader = R"(
      #version 130
      precision mediump float;
      
      uniform vec3 u_color;
      uniform vec3 u_sunPos;
      
      uniform float u_pitch;
      uniform float u_yaw;
      
      varying vec3 v_skyLoc;
      
      vec3 rgb2hsv(vec3 c) {
          vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
          vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
          vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
      
          float d = q.x - min(q.w, q.y);
          float e = 1.0e-10;
          return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
      }

      vec3 hsv2rgb(vec3 c) {
          vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
          vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
          return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
      }
      
      void main() {
          //Workaround for simple radial effect that projects to the cube box
          float dist = (1.0-distance(vec3(v_skyLoc.x, v_skyLoc.y+u_sunPos.y, v_skyLoc.z), u_sunPos)/(2.0*sqrt(2.0)));
          
          vec3 hsv = rgb2hsv(u_color.rgb);
          
          hsv.rb *= u_pitch;
          
          //Gamma cycle correction
          hsv.r -= pow(log((u_pitch)/2.0+1.0), 1.0/2.0);
          
          //Luminosity cycle
          // hsv.b -= (1.0-u_pitch)/3.0;
          
          //Radial gradient
          hsv.b += dist;
          
          vec3 color = hsv2rgb(hsv);
          
          gl_FragColor = vec4(color.rgb, 0.8);
      }
    )";

    Sky(const GLFWvidmode * mode);

    void parseColor();

    int getNbTriangles();

    // void cursorListener(GLFWwindow * window, double posX, double posY);

    // void keyboardListener(GLFWwindow * window, int key, int scancode, int action, int mods);

    // void listenContinouslyToCursor();

    void setParameters();

    void setVariablesLocation();

    void setVariablesData();

    void renderProgram(vector<float> sunPos, float pitch, float yaw);

    void freeProgram();
};

#endif