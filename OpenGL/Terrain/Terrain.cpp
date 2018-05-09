#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <assert.h>
#include <unistd.h>
#include "Terrain.h"

#include "./../Shader/Shader.h"
#include "./../Tools/Matrix/Matrix.h"
using namespace std;

Terrain::Terrain(const GLFWvidmode * mode) {
	this->mode = mode;

	this->terrain.setHoneycomb();
	this->terrain.parseHoneycomb();

	this->program = CreateProgram(vertexShader, fragmentShader);
	setParameters();
	setVariablesLocation();
	setVariablesData();
}


void Terrain::listenContinouslyToCursor() {
    this->yaw += this->traceYaw/this->sensitivity;
    this->pitch += -this->tracePitch/this->sensitivity;
    if(this->yaw < 0)
        this->yaw += 2*M_PI;
    if(this->pitch < 0)
        this->pitch += 2*M_PI;
    this->yaw = fmod(this->yaw, 2*M_PI);
    this->pitch = fmod(this->pitch, 2*M_PI);
};

void Terrain::cursorListener(GLFWwindow * window, double posX, double posY) {
	/**
	 * Function equation f(x)=x^2 will be used to lower the rotation from outer to center of the screen
	 * Where f(x)=x^2 has the domain [0, 2] with the image [0, 4] and [-2, 0] with the image [0, 4]
	 * Where, the derivative increases starting from origins
	 */
	float x = ((posX-mode->width/2.0f)/(mode->width/2.0f));
	float y = ((posY-mode->height/2.0f)/(mode->height/2.0f));

	if(x < 0.0f) {
	    x = pow(x*2.0f, 2.0f);
	    this->traceYaw = x;
	} else {
	    x = -pow(x*2.0f, 2.0f);
	    this->traceYaw = x;
	}

	if(y < 0.0f) {
	    y = pow(y*2.0f, 2.0f);
	    this->tracePitch = y;
	} else {
	    y = -pow(y*2.0f, 2.0f);
	    this->tracePitch = y;
	}
};

void Terrain::keyboardListener(GLFWwindow * window, int key, int scancode, int action, int mods) {
	switch(key) {
		case GLFW_KEY_A : {
			this->roll -= 0.05;
			break;
		};
		case GLFW_KEY_W : {
			this->terrain.speed += this->terrain.partition/8.0f;
			break;
		};
		case GLFW_KEY_S : {
			this->terrain.speed -= this->terrain.partition/8.0f;
			break;
		};
		case GLFW_KEY_D : {
			this->roll += 0.05;
			break;
		};
	};
};

void Terrain::act() {
	vector<float> quaternionRot = fromEuler(-this->pitch/M_PI*180.0f, this->yaw/M_PI*180.0f, 0);

    vector<float> result = transformQuat(vector<float>{0.0f, 0.0f, -this->terrain.speed}, quaternionRot);

    for(unsigned int g = 0; g < this->terrain.mesh.size(); g += 3) {
        this->terrain.mesh[g] += result[0];
        this->terrain.mesh[g+2] += result[2];
        this->terrain.mesh[g+1] = this->terrain.simplex.noise(this->terrain.mesh[g], this->terrain.mesh[g+2])/2.5f;
    }

    this->translation[0] += result[0];
    this->translation[1] += result[1];
    this->translation[2] += result[2];
};

void Terrain::setParameters() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
}

void Terrain::setVariablesLocation() {
	glUseProgram(this->program);

	this->attribPositionLoc = glGetAttribLocation(program, "a_position");

	this->unifCameraLoc = glGetUniformLocation(program, "u_camera");
	this->unifPerspectiveLoc = glGetUniformLocation(program, "u_perspective");

	this->unifLowerColorLoc = glGetUniformLocation(program, "u_gradients[0]");
	this->unifMedianColorLoc = glGetUniformLocation(program, "u_gradients[1]");
	this->unifUpperColorLoc = glGetUniformLocation(program, "u_gradients[2]");
}

void Terrain::setVariablesData() {
	this->perspective.perspective(M_PI/3.0f, this->mode->width/this->mode->height, 0.000001f, 30.0f);

	glUniform3f(unifLowerColorLoc, 120.0f/360.0f, 76.0f/100.0f, 55.0f/100.0f);
	glUniform3f(unifMedianColorLoc, 26.0f/360.0f, 36.0f/100.0f, 65.0f/100.0f);
	glUniform3f(unifUpperColorLoc, 0.0f/360.0f, 2.0f/100.0f, 100.0f/100.0f);

	glEnableVertexAttribArray(this->attribPositionLoc);

	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->terrain.mesh.size()*sizeof(GL_FLOAT), this->terrain.mesh.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(this->attribPositionLoc, 3, GL_FLOAT, false, 0, 0);
};

void Terrain::renderProgram() {
	glUseProgram(this->program);
	this->setParameters();

	glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->terrain.mesh.size()*sizeof(GL_FLOAT), this->terrain.mesh.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(this->attribPositionLoc, 3, GL_FLOAT, false, 0, 0);

	vector<float> quaternionRot = fromEuler(-this->pitch/M_PI*180.0f, this->yaw/M_PI*180.0f, 0);
	this->quaternionMatrix.fromQuat(quaternionRot);

	this->cameraTranslation.translation(translation[0], 1.0f+translation[1], translation[2]);
	this->cameraRotX.rotationX(this->pitch);
	this->cameraRotY.rotationY(this->yaw);
	this->cameraRotZ.rotationZ(this->roll);

	this->cameraMatrix = this->cameraMatrix.inverseMatrix(quaternionMatrix*cameraTranslation);
	glUniformMatrix4fv(unifCameraLoc, 1, false, this->cameraMatrix.matrix.data());
	glUniformMatrix4fv(unifPerspectiveLoc, 1, false, this->perspective.matrix.data());

	glDrawArrays(GL_TRIANGLES, 0, this->terrain.getNbTriangles());

    this->listenContinouslyToCursor();
    this->act();
};

void Terrain::freeProgram() {};