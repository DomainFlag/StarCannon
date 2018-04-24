#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "LOD.h"
#include "./../Shader/Shader.h"
#include "./../Tools/Matrix/Matrix.h"
#include "./../Shader/Shader.cpp"
#include "./../QuadTree/QuadTree.cpp"
#include "./../QuadTree/QuadTree.h"
using namespace std;

LOD::LOD(const GLFWvidmode * mode) {
    this->program = CreateProgram(vertexShader, fragmentShader);
    this->mode = mode;

    this->setParameters();
    this->setVariablesLocation();
    this->setVariablesData();
};

void LOD::setParameters() {
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

void LOD::setVariablesLocation() {
    glUseProgram(this->program);

    this->attribPositionLoc = glGetAttribLocation(this->program, "a_position");
    this->uniformCameraLocation = glGetUniformLocation(this->program, "u_camera");
    this->uniformProjectionLocation = glGetUniformLocation(this->program, "u_projection");
};

void LOD::setVariablesData() {
    glUseProgram(this->program);

    this->projection.perspective(M_PI/3.0f, (float) this->mode->width/this->mode->height, 0.0001, 10);
    this->rotationX.rotationX(M_PI/2);
    this->rotationY.rotationY(0);
    this->rotationZ.rotationZ(0);
    this->translation.translation(0, this->transl, 0);

    this->viewMatrix = this->translation*this->rotationX*this->rotationY*this->rotationZ;

    this->data.reserve(this->quadtree->cols*this->quadtree->cols*3);
    this->quadtree->readProjection(this->data, this->projection, this->viewMatrix);

    glEnableVertexAttribArray(this->attribPositionLoc);

    glGenBuffers(1, &this->positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, this->data.size()*sizeof(float), this->data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(this->attribPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void LOD::renderProgram() {
    glUseProgram(this->program);

    this->translation.translation(0, this->transl, 0);
    this->transl += 0.01;

    this->viewMatrix = this->translation*this->rotationX*this->rotationY*this->rotationZ;

    this->data.clear();
    this->quadtree->readProjection(this->data, this->projection, this->viewMatrix);

    glBufferData(GL_ARRAY_BUFFER, this->data.size()*sizeof(float), this->data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(this->attribPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glUniformMatrix4fv(this->uniformCameraLocation, 1, GL_FALSE, this->viewMatrix.matrix.data());
    glUniformMatrix4fv(this->uniformProjectionLocation, 1, GL_FALSE, this->projection.matrix.data());

    glDrawArrays(GL_TRIANGLES, 0, (int) this->data.size()/3.0f);
}

void LOD::freeProgram() {
	delete this->quadtree;
}
