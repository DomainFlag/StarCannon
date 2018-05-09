#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "Sky.h"
#include "./../Shader/Shader.h"
#include "./../Tools/Matrix/Matrix.h"
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

    glBindBuffer(GL_ARRAY_BUFFER, this->posSkyBuffer);
    glBufferData(GL_ARRAY_BUFFER, this->mesh.size()*sizeof(float), this->mesh.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(this->attribPosSkyLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glUniform1f(this->unifPitchLoc, (sin(-pitch)+0.3));
    glUniform1f(this->unifYawLoc, cos(yaw));
    glUniform3fv(this->unifColorSkyLoc, 1, this->color.data());
    glUniform3fv(this->unifSunSkyLoc, 1, sunPos.data());

    glDrawArrays(GL_TRIANGLES, 0, this->getNbTriangles());
}

void Sky::freeProgram() {}
