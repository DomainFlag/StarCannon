#include "Terrain.h"

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
// #include "./../Shader/Shader.cpp"
#include "./../Tools/Matrix/Matrix.h"
// #include "./../Tools/Matrix/Matrix.cpp"
#include "./../Tools/Noise/Noise.cpp"
using namespace std;

TerrainLayer::TerrainLayer(const GLFWvidmode * mode) {
	this->mode = mode;

	this->terrain.setHoneycomb();
	this->terrain.parseHoneycomb();

	this->program = CreateProgram(vertexShader, fragmentShader);
	setParameters();
	setVariablesLocation();
	setVariablesData();
}


void TerrainLayer::listenContinouslyToCursor() {
    this->yaw += this->traceYaw/360.0f*2.0f;
    this->pitch += -this->tracePitch/360.0f*2.0f;
    if(this->yaw < 0)
        this->yaw += 2*M_PI;
    if(this->pitch < 0)
        this->pitch += 2*M_PI;
    this->yaw = fmod(this->yaw, 2*M_PI);
    this->pitch = fmod(this->pitch, 2*M_PI);
};

void TerrainLayer::cursorListener(GLFWwindow * window, double posX, double posY) {
	/**
	 * Function equation f(x)=1/x will be used to lower the rotation from outer to center of the screen
	 * Where f(x)=1/x has the domain [1/2, 4] with the image [0.25, 2] and [-4, -1/2] with the image [-2, -0.25]
	 * Where, the derivative decreases from 1/2 to 4 and increases from -4 to -1/2
	 */
	float x = ((posX-mode->width/2.0f)/(mode->width/2.0f));
	float y = ((posY-mode->height/2.0f)/(mode->height/2.0f));

	if(x < 0.0f) {
	    x = (x+1.0f)*7.0f/2.0f+1.0f/2.0f;
	    this->traceYaw = 1.0f/x;
	} else {
	    x = -(x-1.0f)*7.0f/2.0f+1.0f/2.0f;
	    this->traceYaw = -1.0f/x;
	}

	if(y < 0.0f) {
	    y = (y+1.0f)*7.0f/2.0f+1.0f/2.0f;
	    this->tracePitch = 1.0f/y;
	} else {
	    y = -(y-1.0f)*7.0f/2.0f+1.0f/2.0f;
	    this->tracePitch = -1.0f/y;
	}
};

void TerrainLayer::keyboardListener(GLFWwindow * window, int key, int scancode, int action, int mods) {
	switch(key) {
		case GLFW_KEY_ESCAPE : {
			glfwDestroyWindow(window);
			break;
		};
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

void TerrainLayer::act() {
	vector<float> quaternionRot = this->quaternionMatrix.fromEuler(-this->pitch/M_PI*360.0f, this->yaw/M_PI*360.0f, 0);

    vector<float> result = this->quaternionMatrix.transformQuat(vector<float>{0.0f, 0.0f, -this->terrain.speed}, quaternionRot);

    for(unsigned int g = 0; g < this->terrain.mesh.size(); g += 3) {
        this->terrain.mesh[g] += result[0];
        this->terrain.mesh[g+2] += result[2];
        this->terrain.mesh[g+1] = this->terrain.simplex.noise(this->terrain.mesh[g], this->terrain.mesh[g+2])/2.5f;
    }

    this->translation[0] += result[0];
    this->translation[1] += result[1];
    this->translation[2] += result[2];
};

void TerrainLayer::setParameters() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_ONE, GL_ZERO);
}

void TerrainLayer::setVariablesLocation() {
	glUseProgram(this->program);

	this->attribPositionLoc = glGetAttribLocation(program, "a_position");

	this->unifViewLoc = glGetUniformLocation(program, "u_model");
	this->unifCameraLoc = glGetUniformLocation(program, "u_camera");
	this->unifPerspectiveLoc = glGetUniformLocation(program, "u_perspective");

	this->unifLowerColorLoc = glGetUniformLocation(program, "u_gradients[0]");
	this->unifMedianColorLoc = glGetUniformLocation(program, "u_gradients[1]");
	this->unifUpperColorLoc = glGetUniformLocation(program, "u_gradients[2]");
}

void TerrainLayer::setVariablesData() {
	this->objectRotX.rotationX(0.0f);
	this->objectRotY.rotationY(0.0f);
	this->objectRotZ.rotationZ(0.0f);

	this->perspective.perspective(1.0f, this->mode->width/this->mode->height, 1.0f, 50.0f);

	glUniform3f(unifLowerColorLoc, 120.0f/360.0f, 76.0f/100.0f, 55.0f/100.0f);
	glUniform3f(unifMedianColorLoc, 26.0f/360.0f, 36.0f/100.0f, 65.0f/100.0f);
	glUniform3f(unifUpperColorLoc, 0.0f/360.0f, 2.0f/100.0f, 100.0f/100.0f);

	glEnableVertexAttribArray(this->attribPositionLoc);

	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->terrain.mesh.size()*sizeof(float), this->terrain.mesh.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(this->attribPositionLoc, 3, GL_FLOAT, false, 0, 0);
};

void TerrainLayer::renderProgram() {
	glUseProgram(this->program);

	glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->terrain.mesh.size()*sizeof(float), this->terrain.mesh.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(this->attribPositionLoc, 3, GL_FLOAT, false, 0, 0);

	this->listenContinouslyToCursor();
	this->act();

	vector<float> quaternion = this->quaternionMatrix.quaternion();
	vector<float> quaternionRot = this->quaternionMatrix.fromEuler(-this->pitch/M_PI*360.0f, this->yaw/M_PI*360.0f, 0);
	this->quaternionMatrix.fromQuat(quaternionRot);

	this->cameraTranslation.translation(translation[0], 1.0f+translation[1], translation[2]);
	this->cameraRotX.rotationX(this->pitch);
	this->cameraRotY.rotationY(this->yaw);
	this->cameraRotZ.rotationZ(this->roll);
	this->objectRotX.rotationX(0.0f);
	this->objectRotY.rotationY(0.0f);
	this->objectRotZ.rotationZ(0.0f);
	this->perspective.perspective(1.0f, this->mode->width/this->mode->height, 1.0f, 50.0f);

	this->modelMatrix = objectRotX*objectRotY*objectRotZ;

	this->cameraMatrix = this->cameraMatrix.inverseMatrix(quaternionMatrix*cameraTranslation);
	glUniformMatrix4fv(unifViewLoc, 1, false, this->modelMatrix.matrix.data());
	glUniformMatrix4fv(unifCameraLoc, 1, false, this->cameraMatrix.matrix.data());
	glUniformMatrix4fv(unifPerspectiveLoc, 1, false, this->perspective.matrix.data());

	glDrawArrays(GL_TRIANGLES, 0, this->terrain.getNbTriangles());
};

void TerrainLayer::freeProgram() {};