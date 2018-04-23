#ifndef SUN
#define SUN

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include "./../Tools/Matrix/Matrix.h"
using namespace std;

class Sun {
public:
    const GLFWvidmode * mode;

    GLuint program;

    float ratio;
    vector<float> mesh;
    float clarity = 64;
    float radiusX = 0.03;
    vector<float> position{0, 0, -1, 0};

    float radiusY;
    vector<float> color{254, 255, 255};

    float rotX = 0, rotY = 0, rotZ = 0;

    Matrix viewMatrix, projection;
    Matrix rotationX, rotationY, rotationZ;

    GLuint posSunBuffer;

    GLint attribPosSunLoc;
    GLint unifColorLoc, unifCameraLoc, unifProjectionLoc, unifSunOriginLoc, unifRadiusLoc;

    string vertexShader = R"(
        #version 130
        attribute vec4 a_sunPos;

        uniform mat4 u_camera;
        uniform mat4 u_projection;

        varying vec3 v_sunPos;

        void main() {
            vec4 pos = u_projection*u_camera*a_sunPos;
            gl_Position = pos;
            
            v_sunPos = a_sunPos.xyz;
        }
    )";

    string fragmentShader = R"(
        #version 130
        precision mediump float;
        
        uniform vec3 u_color;
        uniform float u_radius;
        uniform vec3 u_sunOrigin;
        
        varying vec3 v_sunPos;
        
        void main() {
            float distFromOrigin = distance(u_sunOrigin, v_sunPos);
            float intensity = mix(0.0, u_radius, distFromOrigin)/4.0; //->[0, 0.25]
        
            gl_FragColor = vec4(u_color, 1.0-intensity);
        }
    )";

    Sun(const GLFWvidmode * mode);

    void parseColor();

    void createMesh();

    int getNbTriangles();

    // void cursorListener(GLFWwindow * window, double posX, double posY);

    // void keyboardListener(GLFWwindow * window, int key, int scancode, int action, int mods);

    // void listenContinouslyToCursor();

    void setParameters();

    void setVariablesLocation();

    void setVariablesData();

    void renderProgram();

    void freeProgram();
};

#endif