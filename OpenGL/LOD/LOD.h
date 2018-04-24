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
#include "./../QuadTree/QuadTree.h"
using namespace std;

class LOD {
public:
    const GLFWvidmode * mode;

    GLuint program;

    QuadTree * quadtree = new QuadTree;

    vector<float> data;

    Matrix viewMatrix, projection;
    Matrix rotationX, rotationY, rotationZ, translation;

    float transl = -10.0;

    GLuint positionBuffer;
    GLint attribPositionLoc, uniformProjectionLocation, uniformCameraLocation;

    string vertexShader = R"(
    	#version 130
	    attribute vec4 a_position;
	    
	    uniform mat4 u_projection;
	    uniform mat4 u_camera;
	    
	    varying float v_depth;
	    varying vec2 v_tex;
	    varying float v_far;
	    
	    void main() {
	        vec4 projectionView = u_projection*u_camera*a_position;
	        
	        gl_Position = projectionView;
	        
	        v_depth = (a_position.y+2.0)/2.0;
	        v_tex = vec2(((a_position.x+a_position.z)/4.0+0.5)*5.0, 1.0-v_depth);
	        v_far = 10.0-log(distance(gl_Position.xyz, vec3(0, 0, 0)));
	    }
    )";

    string fragmentShader = R"(
	  	#version 130
	    precision mediump float;
	    
	    uniform sampler2D u_texture;
	    
	    varying vec2 v_tex;
	    varying float v_depth;
	    varying float v_far;
	    
	    void main() {
	        gl_FragColor = vec4(0.147, 0.572, 0.792, 1.0);            
	        gl_FragColor.rgb *= v_depth;
            gl_FragColor.w = v_far;
            
	        // if(v_depth >= 0.05) {
	        //     gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0);
	        //     gl_FragColor = texture2D(u_texture, v_tex);
	        //
	        //     gl_FragColor.rgb *= v_depth;
	        //
	        //     gl_FragColor.w = v_far;
	        // } else {
	        //     gl_FragColor = vec4(0.147, 0.572, 0.792, 1.0);
	        // }
	    }
    )";

    LOD(const GLFWvidmode * mode);

    void setParameters();

    void setVariablesLocation();

    void setVariablesData();

    void renderProgram();

    void freeProgram();
};

#endif