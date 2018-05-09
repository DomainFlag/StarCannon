#ifndef SHOT
#define SHOT

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <list>
#include "./../Tools/Matrix/Matrix.h"
using namespace std;

class Shot {
public:
	GLuint program;
	const GLFWvidmode * mode;

	vector<float> transl;
	vector<float> rotat{M_PI*3/2, M_PI*3/2, 0};
    vector<float> direction;
	float tail = 0.8;
	vector<float> color{114, 236, 254};
	float errorY = 0.005;
	float amplitude = 0.4;

	int nbParticles = 800;
	float range = 0.8;

	vector<float> particles;

    GLint attribPosLoc, unifModelViewLoc, unifProjectionLoc, unifColorLoc, unifTailLoc, unifScalarTailNormalizerLoc;

    GLuint posBuffer;

    Matrix rotX, rotY, rotZ, translation;
    Matrix modelView, projection;

    string vertexShader = R"(
        #version 130

	    attribute vec4 a_position;
	    
	    uniform mat4 u_model;
	    uniform mat4 u_projection;
	    uniform float u_tail;
	    uniform float u_scalarTailNormalizer;
	    
	    varying float v_far;
	    varying float v_dispersion;
	    
	    void main() {
	        gl_PointSize = 1.5;
	        
	        vec4 pos = u_projection*u_model*vec4(a_position.xyz, 1.0);
	        gl_Position = pos;
	        
	        float len = length(vec3(0, a_position.xy))/a_position.w;
	        
	        v_far = 1.0/exp(-2.0*(a_position.z+u_tail/2.0))/u_scalarTailNormalizer;
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
            hsv.b -= v_dispersion;
            vec3 color = hsv2rgb(hsv);

            color.r = 0.8;
            
            gl_FragColor = vec4(color, 1.0);
        }
    )";

    Shot(const GLFWvidmode * mode, vector<float> direction, vector<float> transl, vector<float> rotat);

    void initializeShot();

    void normalizeColor();

    vector<float> initializeParticle();

    vector<float> projectParticleProperties(float x);

    float bell(float x);

    float getMaxBell(float x);

    void act();

    void setParameters();

    void setVariablesLocation();

    void setVariablesData();

    void renderProgram();

    void freeProgram();
};

#endif