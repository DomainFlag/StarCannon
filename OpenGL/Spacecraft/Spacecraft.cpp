#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include "Spacecraft.h"
#include "./../Shader/Shader.h"
#include "./../Tools/Matrix/Matrix.h"
// #include "./../Tools/Matrix/Matrix.cpp"
// #include "./../Shader/Shader.cpp"
using namespace std;

Spacecraft::Spacecraft() {
   this->program = CreateProgram(this->vertexShader, this->fragmentShader);

   this->cube = setCube(-0.04, -0.04, 0.08);
   setColors();

   setParameters();
   setVariablesLocation();
   setVariablesData();
}

vector<float> Spacecraft::setCube(float x1, float y1, float length) {
   vector<float> mesh = {
      //Red
      x1, y1, -length/2,
      x1+length, y1+length, -length/2,
      x1, y1+length, -length/2,
      x1+length, y1, -length/2,
      x1+length, y1+length, -length/2,
      x1, y1, -length/2,

      //Green
      x1, y1, length/2,
      x1, y1, -length/2,
      x1, y1+length, -length/2,
      x1, y1+length, -length/2,
      x1, y1+length, length/2,
      x1, y1, length/2,

      //Blue
      x1+length, y1, length/2,
      x1+length, y1+length, length/2,
      x1+length, y1+length, -length/2,
      x1+length, y1+length, -length/2,
      x1+length, y1, -length/2,
      x1+length, y1, length/2,

      //Yellow
      x1, y1+length, length/2,
      x1+length, y1+length, -length/2,
      x1+length, y1+length, length/2,
      x1+length, y1+length, -length/2,
      x1, y1+length, length/2,
      x1, y1+length, -length/2,

      //Light Blue
      x1, y1, length/2,
      x1+length, y1, length/2,
      x1+length, y1, -length/2,
      x1+length, y1, -length/2,
      x1, y1, -length/2,
      x1, y1, length/2,

      //Rose
      x1, y1, length/2,
      x1, y1+length, length/2,
      x1+length, y1+length, length/2,
      x1+length, y1+length, length/2,
      x1+length, y1, length/2,
      x1, y1, length/2
   };

   return mesh;
}

void Spacecraft::setColors() {
   vector<float> colors = {
      1.0f, 0, 0,
      0, 1.0f, 0,
      0, 0, 1.0f,
      1.0f, 1.0f, 0,
      0, 1.0f, 1.0f,
      1.0f, 0, 1.0f
   };

   for(int g = 0; g < colors.size(); g += 3) {
      for(int h = 0; h < 6; h++) {
         this->palette.push_back(colors[g]);
         this->palette.push_back(colors[g+1]);
         this->palette.push_back(colors[g+2]);
      }
   }
}

void Spacecraft::listenContinouslyToCursor() {}

void Spacecraft::act() {}

void Spacecraft::setParameters() {
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
   glDepthFunc(GL_LESS);
   glDisable(GL_BLEND);
}

void Spacecraft::setVariablesLocation() {
   this->attribPositionLoc = glGetAttribLocation(this->program, "a_position");
   this->attribColorLoc = glGetAttribLocation(this->program, "a_color");
   this->unifCameraLoc = glGetUniformLocation(this->program, "u_camera");
   this->unifPerspectiveLoc = glGetUniformLocation(this->program, "u_perspective");
}

void Spacecraft::setVariablesData() {
   glUseProgram(this->program);

   glEnableVertexAttribArray(this->attribPositionLoc);
   glEnableVertexAttribArray(this->attribColorLoc);

   glGenBuffers(1, &this->positionBuffer);
   glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
   glBufferData(GL_ARRAY_BUFFER, (int) this->cube.size()*sizeof(float), this->cube.data(), GL_STATIC_DRAW);
   glVertexAttribPointer(this->attribPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

   glGenBuffers(1, &this->colorBuffer);
   glBindBuffer(GL_ARRAY_BUFFER, this->colorBuffer);
   glBufferData(GL_ARRAY_BUFFER, (int) this->palette.size()*sizeof(float), this->palette.data(), GL_STATIC_DRAW);
   glVertexAttribPointer(this->attribColorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Spacecraft::renderProgram() {
   glUseProgram(this->program);

   this->rotationX.rotationX(this->rotX);
   glUniformMatrix4fv(this->unifCameraLoc, 1, GL_FALSE, this->rotationX.matrix.data());

   glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
   glVertexAttribPointer(this->attribPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

   glBindBuffer(GL_ARRAY_BUFFER, this->colorBuffer);
   glVertexAttribPointer(this->attribColorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

   glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Spacecraft::freeProgram() {}