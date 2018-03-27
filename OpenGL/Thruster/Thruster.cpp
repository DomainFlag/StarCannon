#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "./Thruster.h"
#include "./../Shader/Shader.h"
#include "./../Tools/Matrix/Matrix.h"
using namespace std;

Thruster::Thruster(const GLFWvidmode * mode) {
   this->program = CreateProgram(this->vertexShader, this->fragmentShader);
   this->initThruster();
   this->mode = mode;

   setParameters();
   setVariablesLocation();
   setVariablesData();
};

void Thruster::initThruster() {
   float x, y, z, max;
   Matrix rotationZ;
   for(int g = 0; g < this->nbParticles; g++) {

      z = (float) rand()/RAND_MAX*this->tail;
      max = this->bell(z);
      y = (float) rand()/RAND_MAX*max;

      float rotation = (float) rand()/RAND_MAX*2*M_PI;      
      rotationZ.rotationZ(rotation);

      vector<float> data = rotationZ.multiplyVector(vector<float>{0, y, z, 0});

      //Only xyz values
      for(int g = 0; g < data.size()-1; g++){
         this->particles.push_back(data[g]);
      }
   }
};

float Thruster::bell(float x) {
   return ((-exp(x*x)+1.0f)/(exp(1.0f)-1.0f)+1.0f)/this->bellfat;
};

void Thruster::act() {
   Matrix rotationZ;
   for(int g = 0; g < this->nbParticles*3; g += 3)
      if(this->particles[g+2] > -(this->tail+1.0)) {
         this->particles[g+2] = this->tail+this->speed*(float)rand()/RAND_MAX*3.0f;
         float max = this->bell(this->particles[g+2]);
         this->particles[g+1] = (float)rand()/RAND_MAX*max;

         float rotation = (float)rand()/RAND_MAX*2*M_PI;

         rotationZ.rotationZ(rotation);

         vector<float> data = rotationZ.multiplyVector(vector<float>{0, this->particles[g+1], this->particles[g+2], 0});

         this->particles[g] = data[0];
         this->particles[g+1] = data[1];
         this->particles[g+2] = data[2];
      } else {
         float proportionX = this->particles[g]/this->bell(this->particles[g+2]);
         float proportionY = this->particles[g+1]/this->bell(this->particles[g+2]);

         this->particles[g+2] += this->speed*(float)rand()/RAND_MAX;
         this->particles[g] = proportionX*this->bell(this->particles[g+2]);
         this->particles[g+1] = proportionY*this->bell(this->particles[g+2]);
      }
};

void Thruster::setParameters() {
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);
   glEnable(GL_PROGRAM_POINT_SIZE);

   glDisable(GL_CULL_FACE);

   glEnable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
};

void Thruster::setVariablesLocation() {
   this->attribPositionLoc = glGetAttribLocation(this->program, "a_position");
   this->unifCameraLoc = glGetUniformLocation(this->program, "u_camera");
   this->unifPerspectiveLoc = glGetUniformLocation(this->program, "u_perspective");
};

void Thruster::setVariablesData() {
   glUseProgram(this->program);

   glEnableVertexAttribArray(this->attribPositionLoc);
   // Buffer for particles' position
   glGenBuffers(1, &positionBuffer);
   glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
   glBufferData(GL_ARRAY_BUFFER, this->particles.size()*sizeof(float), this->particles.data(), GL_DYNAMIC_DRAW);
   glVertexAttribPointer(this->attribPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
};

void Thruster::renderProgram() {
   glUseProgram(this->program);
   this->act();

   this->viewMatrix = this->cameraRotX*this->cameraRotY*this->cameraRotZ*this->cameraTranslation;
   this->cameraMatrix = this->viewMatrix.inverseMatrix(this->viewMatrix);
   this->perspectiveMatrix.perspective(1.0f, this->mode->width/this->mode->height, 1.0f, 20.0f);

   glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
   glBufferData(GL_ARRAY_BUFFER, this->particles.size()*sizeof(float), this->particles.data(), GL_DYNAMIC_DRAW);
   glVertexAttribPointer(this->attribPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

   glUniformMatrix4fv(this->unifCameraLoc, 1, GL_FALSE, this->cameraMatrix.matrix.data());
   glUniformMatrix4fv(this->unifPerspectiveLoc, 1, GL_FALSE, this->perspectiveMatrix.matrix.data());

   glDrawArrays(GL_POINTS, 0, this->particles.size()/3.0f);
};

void Thruster::freeProgram() {};