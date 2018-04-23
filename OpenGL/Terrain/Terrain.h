#ifndef TERRAIN
#define TERRAIN

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <assert.h>
#include <unistd.h>
#include <SOIL/SOIL.h>
#include "./../Shader/Shader.h"
#include "./../Tools/Matrix/Matrix.h"
#include "./../Tools/Noise/Noise.h"
using namespace std;

class Vector {
public:
    float x, y, z;

    Vector(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    vector<float> getVector() {
        return vector<float>{this->x, this->y, this->z};
    }
};

class Terrain {
public:
    Simplex simplex;

    vector<float> mesh;
    vector<vector<Vector>> honeycomb;

    unsigned int meshNbElements = 0;

    float partition = 1.0f/15.0f;
    float speed = this->partition/3.0f;

    Terrain() {};

    void setHoneycomb() {
        simplex.init();
        for(float z = -3.0f*sqrt(2.0f)-this->partition; z < 3.0f*sqrt(2.0f)+this->partition; z += this->partition) {
            vector<Vector> layer;
            for(float x = -3.0f*sqrt(2.0f)-this->partition; x < 3.0f*sqrt(2.0f)+this->partition; x += this->partition) {
                float coordX = x;
                float coordY = (float) simplex.noise(x, z)/2.5; //[-0.4, 0.4]
                float coordZ = z;
                layer.push_back(Vector(coordX, coordY, coordZ));
            }
            this->honeycomb.push_back(layer);
        }
    };

    void parseHoneycomb() {
        vector<vector<int>> cycle = {
                vector<int>{0, 0},
                vector<int>{0, 1},
                vector<int>{1, 1},
                vector<int>{1, 1},
                vector<int>{1, 0},
                vector<int>{0, 0}
        };

        for(unsigned int g = 0; g < this->honeycomb.size()-1; g++) {
            for(unsigned int h = 0; h < this->honeycomb[g].size()-1; h++) {
                for(unsigned int i = 0; i < cycle.size(); i++) {
                    vector<float> vec = (this->honeycomb[g+cycle[i][0]][h+cycle[i][1]]).getVector();

                    this->mesh.push_back(vec[0]);
                    this->mesh.push_back(vec[1]);
                    this->mesh.push_back(vec[2]);
                }
                meshNbElements += 6;
            }
        }
    };

    unsigned int getNbTriangles() {
        return meshNbElements;
    };
};

class TerrainLayer {
public:
    Terrain terrain;

    vector<float> translation = decltype(translation)(3, 0);
    float yaw = 0;
    float pitch = 0;
    float roll = 0;
    float traceYaw = 0;
    float tracePitch = 0;
    float traceRoll = 0;

    const GLFWvidmode * mode;

    unsigned int program;

    unsigned int positionBuffer;

    GLint attribPositionLoc;
    GLint unifCameraLoc, unifViewLoc, unifPerspectiveLoc;
    GLint unifLowerColorLoc, unifMedianColorLoc, unifUpperColorLoc;

    Matrix modelMatrix, cameraMatrix, viewMatrix;

    Matrix quaternionMatrix, cameraTranslation, cameraRotX, cameraRotY, cameraRotZ;
    Matrix objectRotX, objectRotY, objectRotZ;
    Matrix perspective;

    string vertexShader = R"(
		#version 130
        attribute vec4 a_position;
        varying float v_depth;
        varying float v_far;
        uniform mat4 u_model;
        uniform mat4 u_camera;
        uniform mat4 u_perspective;
        void main() {
            vec4 camera = u_camera*u_model*a_position;
            vec4 result = u_perspective*camera;
            gl_Position = result;
            v_depth = (a_position.y+1.0/2.5)*5.0/4.0; //->[0, 1]
            v_far = -camera.z; //->[0.0, inf]
        }
	)";

    string fragmentShader = R"(
		#version 130
        precision mediump float;
        varying float v_depth;
        varying float v_far;
        uniform vec3 u_gradients[3];
        
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
        
        //HSV interpolation
        vec3 colorInterpolation() {
            float part;
            if(v_depth < 0.7) {
                part = v_depth/(7.0/10.0);
                return u_gradients[0]*(1.0-part)+u_gradients[1]*part;
            } else {
                part = (v_depth-0.7)/(3.0/10.0);
                return u_gradients[1]*(1.0-part)+u_gradients[2]*part;
            }
        }

        //Main
        void main() {
            vec3 color = hsv2rgb(colorInterpolation());
            vec4 depth_color = vec4(color.rgb, 1.0); //->[0, 0.2]
            // vec4 depth_color = vec4(color.rgb, 0.8+v_depth/5.0); //->[0, 0.2]
            vec4 far_color = vec4(depth_color.rgb, depth_color.a-pow(sqrt(exp(v_far)-1.0), 2.0)/100.0); //->(sqrt(e^x)-1)^2
            gl_FragColor = far_color;
        }
	)";

    TerrainLayer(const GLFWvidmode * mode);

    void listenContinouslyToCursor();

    void cursorListener(GLFWwindow * window, double posX, double posY);

    void keyboardListener(GLFWwindow * window, int key, int scancode, int action, int mods);

    void act();

    void setParameters();

    void setVariablesLocation();

    void setVariablesData();

    void renderProgram();

    void freeProgram();
};

#endif