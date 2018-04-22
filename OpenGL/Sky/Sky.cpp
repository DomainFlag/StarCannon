#include "Sky.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "./../Shader/Shader.h"
#include "./../Tools/Matrix/Matrix.h"
#include "./../Shader/Shader.cpp"
#include "./../Tools/Matrix/Matrix.cpp"
using namespace std;

Sky::Sky(const GLFWvidmode * mode) {
    this->program = CreateProgram(vertexShader, fragmentShader);
    this->mode = mode;

    this->parseColor();

    this->setParameters();
    this->setVariablesLocation();
    this->setVariablesData();
};

void Sky::parseColor() {
  for(int g = 0; g < this->color.size(); g++)
    this->color[g] /= 255;
};

// void Sky::act(float now) {
//   now *= 0.001;

//   float deltaTime = (now-this->past)/5.0;
//   this->rotationX = (this->rotationX-deltaTime)%(-2*M_PI);

//   this->past = now;
// };

int Sky::getNbTriangles() {
  return (int) this->mesh.size()/3;
};


// void Sky::cursorListener(GLFWwindow * window, double posX, double posY) {
//     /**
//     * Function equation f(x)=1/x will be used to lower the rotation from outer to center of the screen
//     * Where f(x)=1/x has the domain [1/2, 4] with the image [0.25, 2] and [-4, -1/2] with the image [-2, -0.25]
//     * Where, the derivative decreases from 1/2 to 4 and increases from -4 to -1/2
//     */
//     float x = ((posX-mode->width/2.0f)/(mode->width/2.0f));
//     float y = ((posY-mode->height/2.0f)/(mode->height/2.0f));

//     if(x < 0.0f) {
//         x = (x+1.0f)*7.0f/2.0f+1.0f/2.0f;
//         this->traceYaw = 1.0f/x;
//     } else {
//         x = -(x-1.0f)*7.0f/2.0f+1.0f/2.0f;
//         this->traceYaw = -1.0f/x;
//     }

//     if(y < 0.0f) {
//         y = (y+1.0f)*7.0f/2.0f+1.0f/2.0f;
//         this->tracePitch = 1.0f/y;
//     } else {
//         y = -(y-1.0f)*7.0f/2.0f+1.0f/2.0f;
//         this->tracePitch = -1.0f/y;
//     }
// };

// void Sky::keyboardListener(GLFWwindow * window, int key, int scancode, int action, int mods) {

// };

// void Sky::listenContinouslyToCursor() {
//     this->yaw += this->traceYaw/360.0f*2.0f;
//     this->pitch += -this->tracePitch/360.0f*2.0f;

//     if(this->yaw < 0)
//         this->yaw += 2*M_PI;

//     if(this->pitch < 0)
//         this->pitch += 2*M_PI;

//     this->yaw = fmod(this->yaw, 2*M_PI);
//     this->pitch = fmod(this->pitch, 2*M_PI);
// };

void Sky::setParameters() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void Sky::setVariablesLocation() {
    glUseProgram(this->program);

    this->attribPosSkyLoc = glGetAttribLocation(this->program, "a_skyPos");
    this->unifPitchLoc = glGetUniformLocation(this->program, "u_pitch");
    this->unifYawLoc = glGetUniformLocation(this->program, "u_yaw");
    this->unifColorSkyLoc = glGetUniformLocation(this->program, "u_color");
    this->unifSunSkyLoc = glGetUniformLocation(this->program, "u_sunPos");
}

void Sky::setVariablesData() {
    glUseProgram(this->program);

    glEnableVertexAttribArray(this->attribPosSkyLoc);

    glGenBuffers(1, &this->posSkyBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->posSkyBuffer);
    glBufferData(GL_ARRAY_BUFFER, this->mesh.size()*sizeof(float), this->mesh.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(this->attribPosSkyLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Sky::renderProgram(vector<float> sunPos, float pitch, float yaw) {
    glUseProgram(this->program);

    // this->rotationX.rotationX(this->rotX/360*2*M_PI);
    // this->rotationY.rotationY(this->rotY/360*2*M_PI);
    // this->rotationZ.rotationZ(this->rotZ/360*2*M_PI);

    // Matrix viewMatrix = rotationX*rotationY*rotationZ;

    // vector<float> vector = this->viewMatrix.multiplyVector(sunPos);

    glBindBuffer(GL_ARRAY_BUFFER, this->posSkyBuffer);
    glBufferData(GL_ARRAY_BUFFER, this->mesh.size()*sizeof(float), this->mesh.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(this->attribPosSkyLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glUniform1f(this->unifPitchLoc, pitch);
    glUniform1f(this->unifYawLoc, yaw);
    glUniform3fv(this->unifColorSkyLoc, 1, this->color.data());
    glUniform3fv(this->unifSunSkyLoc, 1, sunPos.data());

    glDrawArrays(GL_TRIANGLES, 0, this->getNbTriangles());
}

void Sky::freeProgram() {}
