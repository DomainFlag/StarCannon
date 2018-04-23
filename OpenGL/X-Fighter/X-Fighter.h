#ifndef X_FIGHTER
#define X_FIGHTER

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "./../Tools/Matrix/Matrix.h"
#include "./../Tools/ObjReader/OBJReader.h"
using namespace std;


class X_Fighter {
public:
	GLuint program;

    const GLFWvidmode * mode;

    Data data;

    GLuint posBuffer, texBuffer;
    GLint attribPosLoc, attribTexLoc, unifCameraLoc, unifProjectionLoc;

    vector<float> translation{0, 0, -25};
    vector<float> rotation{0, 0, 0};

    Matrix projection;
    Matrix modelView, transl;

    vector<GLuint> textures;

    string vertexShader = R"(
        #version 130

        attribute vec4 a_position;
        attribute vec3 a_texture;

        uniform mat4 u_camera;
        uniform mat4 u_projection;

        varying vec3 v_texture;

        void main() {
            vec4 projection = u_projection*u_camera*a_position;
            gl_Position = projection;
            
            v_texture = a_texture;
        }
    )";

    string fragmentShader = R"(
        #version 130

        uniform sampler2D u_texture[5];
        varying vec3 v_texture;

        void main() {
            int texUnit = int(v_texture.z);
            if(texUnit == 0)
                gl_FragColor = texture2D(u_texture[0], vec2(1, 1)-v_texture.xy);
            else if(texUnit == 1)
                gl_FragColor = texture2D(u_texture[1], vec2(1, 1)-v_texture.xy);
            else if(texUnit == 2)
                gl_FragColor = texture2D(u_texture[2], vec2(1, 1)-v_texture.xy);
            else if(texUnit == 3)
                gl_FragColor = texture2D(u_texture[3], vec2(1, 1)-v_texture.xy);
            else if(texUnit == 4)
                gl_FragColor = texture2D(u_texture[4], vec2(1, 1)-v_texture.xy);
            else  gl_FragColor = texture2D(u_texture[3], v_texture.xy);
        } 
    )";

    X_Fighter(const GLFWvidmode * mode);

    void setParameters();

    void setVariablesLocation();

    void setVariablesData();

    void renderProgram();

    void freeProgram();
};

#endif