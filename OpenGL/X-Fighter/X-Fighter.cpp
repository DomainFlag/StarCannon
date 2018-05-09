#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <SOIL/SOIL.h>
#include "./X-Fighter.h"
#include "./../Tools/ObjReader/ObjReader.h"
#include "./../Shader/Shader.h"
#include "./../Tools/Matrix/Matrix.h"

using namespace std;

X_Fighter::X_Fighter(const GLFWvidmode * mode) {
	this->program = CreateProgram(this->vertexShader, this->fragmentShader);
    this->mode = mode;

    readDir(this->data, "./../Tools/Obj/X-Fighter/");

    this->setParameters();
    this->setVariablesLocation();
    this->setVariablesData();
    this->renderProgram();
}

void X_Fighter::setParameters() {
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_PROGRAM_POINT_SIZE);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
};

void X_Fighter::setVariablesLocation() {
   glUseProgram(this->program);

   this->attribPosLoc = glGetAttribLocation(this->program, "a_position");
   this->attribTexLoc = glGetAttribLocation(this->program, "a_texture");
   this->unifCameraLoc = glGetUniformLocation(this->program, "u_camera");
   this->unifProjectionLoc = glGetUniformLocation(this->program, "u_projection");
};

void X_Fighter::setVariablesData() {
	this->projection.perspective(M_PI/3, (float) this->mode->width/this->mode->height, 0.001, 30);

	glEnableVertexAttribArray(this->attribPosLoc);
	glGenBuffers(1, &this->posBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->posBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->data.geometricVertices.size()*sizeof(float), this->data.geometricVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(this->attribPosLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(this->attribTexLoc);
	glGenBuffers(1, &this->texBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->texBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->data.textureVertices.size()*sizeof(float), this->data.textureVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(this->attribTexLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	this->scaling.scaling(0.03, 0.03, 0.03);

	int width, height;
	int counter = 0;
	this->textures.resize(this->data.textures.size());

	for(vector<GLuint>::iterator it = this->textures.begin(); it != this->textures.end(); it++) {
		string texLocString = string("u_texture[") + to_string(this->data.textures[counter].texUnit) + string("]");
		GLint unifTexLoc = glGetUniformLocation(this->program, texLocString.data());
		glUniform1i(unifTexLoc, counter);
		glActiveTexture(GL_TEXTURE0+counter);

		glGenTextures(1, &*it);
		glBindTexture(GL_TEXTURE_2D, *it);

		unsigned char * image = SOIL_load_image(("./../Tools/Obj/X-Fighter/" + this->data.textures[counter].name).c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		if(image == NULL) {
			cout << SOIL_last_result() << endl;
			exit(1);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);

		counter++;
	}
};

void X_Fighter::renderProgram() {
	glUseProgram(this->program);
	this->setParameters();

	this->transl.translation(this->translation[0]+this->position[0], 
		this->translation[1]+this->position[1], 
		this->translation[2]+this->position[2]);

	vector<float> quat = fromEuler(this->rotation[0]/2/M_PI*360, this->rotation[1]/2/M_PI*360, this->rotation[2]/2/M_PI*360);
	this->modelView.fromQuat(quat);
	this->modelView = this->scaling*this->modelView*this->transl;

	this->lookAt.lookAt(vector<float>{
		this->transl.matrix[12], this->transl.matrix[13]+0.5f, this->transl.matrix[14]-1.5f
	}, this->translation, vector<float>{0, 1, 0});

	this->lookAt = this->modelView*this->lookAt;

	glBindBuffer(GL_ARRAY_BUFFER, this->posBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->data.geometricVertices.size()*sizeof(float), this->data.geometricVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(this->attribPosLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, this->texBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->data.textureVertices.size()*sizeof(float), this->data.textureVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(this->attribTexLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glUniformMatrix4fv(this->unifCameraLoc, 1, GL_FALSE, this->lookAt.matrix.data());
	glUniformMatrix4fv(this->unifProjectionLoc, 1, GL_FALSE, this->projection.matrix.data());

	glDrawArrays(GL_TRIANGLES, 0, (int) this->data.geometricVertices.size()/3.0f);
};

void X_Fighter::freeProgram() {};