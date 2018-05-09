#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "Sun.h"
#include "./../Shader/Shader.h"
#include "./../Tools/Matrix/Matrix.h"
using namespace std;

Sun::Sun(const GLFWvidmode * mode) {
    this->program = CreateProgram(vertexShader, fragmentShader);
    this->mode = mode;

    this->ratio = (float) mode->width/mode->height;
    this->radiusY = this->radiusX*this->ratio;

    this->parseColor();
    this->createMesh();

    this->setParameters();
    this->setVariablesLocation();
    this->setVariablesData();
};

void Sun::parseColor() {
  for(int g = 0; g < this->color.size(); g++)
    this->color[g] /= 255;
};

void Sun::createMesh() {
    float rotation = 0;
    float partition = 2*M_PI/this->clarity;

    float c1 = this->radiusX, s1 = 0;
    float c2, s2;
    for(int t = 0; t <= this->clarity; t++) {
        rotation += partition;

        c2 = cos(rotation)*this->radiusX;
        s2 = sin(rotation)*this->radiusY;

        this->mesh.push_back(c1);
        this->mesh.push_back(s1);
        this->mesh.push_back(-1);

        this->mesh.push_back(c2);
        this->mesh.push_back(s2);
        this->mesh.push_back(-1);

        this->mesh.push_back(0);
        this->mesh.push_back(0);
        this->mesh.push_back(-1);

        c1 = c2;
        s1 = s2;
    }
};

int Sun::getNbTriangles() {
  return (int) this->mesh.size()/3;
};

// void Sun::act(float now) {
//   now *= 0.001;

//   float deltaTime = (now-this->past)/5.0;
//   this->rotationX = (this->rotationX-deltaTime)%(-2*M_PI);

//   this->past = now;
// };

void Sun::setParameters() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void Sun::setVariablesLocation() {
    glUseProgram(this->program);

    this->attribPosSunLoc = glGetAttribLocation(this->program, "a_sunPos");

    this->unifColorLoc = glGetUniformLocation(this->program, "u_color");
    this->unifCameraLoc = glGetUniformLocation(this->program, "u_camera");
    this->unifProjectionLoc = glGetUniformLocation(this->program, "u_projection");
    this->unifSunOriginLoc = glGetUniformLocation(this->program, "u_sunOrigin");
    this->unifRadiusLoc = glGetUniformLocation(this->program, "u_radius");
}

void Sun::setVariablesData() {
    glUseProgram(this->program);

    this->projection.perspective(M_PI/3.0f, mode->width/mode->height, 0.0001, 10);

    glEnableVertexAttribArray(this->attribPosSunLoc);

    glGenBuffers(1, &this->posSunBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->posSunBuffer);
    glBufferData(GL_ARRAY_BUFFER, this->mesh.size()*sizeof(float), this->mesh.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(this->attribPosSunLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Sun::renderProgram() {
    glUseProgram(this->program);

    this->rotX = fmod(this->rotX+0.002, 2*M_PI);

    this->rotationX.rotationX(this->rotX);
    this->rotationY.rotationY(this->rotY);
    this->rotationZ.rotationZ(this->rotZ);

    this->viewMatrix = rotationX*rotationY*rotationZ;

    glBindBuffer(GL_ARRAY_BUFFER, this->posSunBuffer);
    glBufferData(GL_ARRAY_BUFFER, this->mesh.size()*sizeof(float), this->mesh.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(this->attribPosSunLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glUniformMatrix4fv(this->unifCameraLoc, 1, false, this->viewMatrix.matrix.data());
    glUniformMatrix4fv(this->unifProjectionLoc, 1, false, this->projection.matrix.data());

    glUniform3fv(this->unifColorLoc, 1, this->color.data());
    glUniform4fv(this->unifSunOriginLoc, 1, this->position.data());
    glUniform1f(this->unifRadiusLoc, this->radiusX);

    glDrawArrays(GL_TRIANGLES, 0, this->getNbTriangles());
}

void Sun::freeProgram() {}
