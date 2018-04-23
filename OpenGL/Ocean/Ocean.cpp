#include "Ocean.h"

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
#include "./../Tools/Matrix/Matrix.cpp"
#include "./../Tools/Matrix/Matrix.h"
using namespace std;

static unsigned int CompileShader(unsigned int type, const string& source) {
	unsigned int id = glCreateShader(type);
	const char * src = source.c_str();
	glShaderSource(id, 1, &src, NULL);
	glCompileShader(id);

	int result; 
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if(result == GL_FALSE) {
		std::cout << "failed to compile shader" << std::endl;
		GLint maxLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

		int iter;
		for (vector<GLchar>::const_iterator iter = errorLog.begin(); iter != errorLog.end(); ++iter)
			cout << *iter;

		glDeleteShader(id);
	}
	return id;
}

static int CreateProgram(const string& vertexShader, const string& fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDetachShader(program,vs);
	glDetachShader(program,fs);
	glDeleteShader(vs); 
	glDeleteShader(fs);

	return program;
}


OceanLayer::OceanLayer(const GLFWvidmode * mode) {
		this->mode = mode;
		this->projection.projection(mode->width, mode->height, 1000.0f);
		this->translation.translation(0.0f, 0.0f, 0.0f);

		this->ocean.setHoneycomb(this->mode);
		this->ocean.parseHoneycomb();

		this->rotationX.rotationX(0.0f);
		this->rotationY.rotationX(0.0f);
		this->rotationZ.rotationX(0.0f);

		srand(time(NULL));

		this->program = CreateProgram(vertexShader, fragmentShader);
		setDataLocations();
		setData();

}

void OceanLayer::setSettings() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OceanLayer::setDataLocations() {
	this->vertexPositionLocation = glGetAttribLocation(program, "a_position");
	this->uniformMatrixLocation = glGetUniformLocation(program, "u_matrix");
	this->uniformRotationLocation = glGetUniformLocation(program, "u_rotation");
	this->uniformDepthCurrentLocation = glGetUniformLocation(program, "u_depth_current");
	this->uniformDepthMaxLocation = glGetUniformLocation(program, "u_depth_max");
	this->uniformTranslation = glGetUniformLocation(program, "u_translation");
	this->uniformRandomLocation = glGetUniformLocation(program, "u_random");
}

void OceanLayer::setData() {
	glGenBuffers(1, &this->positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->ocean.mesh.size()*sizeof(float), this->ocean.mesh.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(this->vertexPositionLocation);
	glVertexAttribPointer(this->vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char * image = SOIL_load_image("./../Tools/Textures/water1.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	if(image == NULL) {
		cout << "Error while loading the texture";
		exit(1);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
}

void OceanLayer::renderOcean() {
	glUseProgram(this->program);

	// if(translation->matrix[13]-ocean.speed > mode->height/2.0f)
	//     translation->translation(0, 0, 0);
	// else
	// 	translation->translation(0, translation->matrix[13]-ocean.speed, 0);

	this->viewMatrix = this->translation*this->rotationX*this->rotationY*this->rotationZ;
	glUniformMatrix4fv(this->uniformMatrixLocation, 1, GL_FALSE, this->projection.matrix.data());
	glUniformMatrix4fv(this->uniformRotationLocation, 1, GL_FALSE, this->viewMatrix.matrix.data());

	glUniform1f(this->uniformTranslation, this->translation.matrix[13]/mode->height*2.0-1.0);
	glUniform1f(this->uniformDepthCurrentLocation, 200);
	glUniform1f(this->uniformDepthMaxLocation, 1000);
	glUniform1f(this->uniformRandomLocation, noise.getNoise());

	glDrawArrays(GL_TRIANGLES, 0, this->ocean.getNbTriangles());
}

void OceanLayer::freeOcean() {}