#include "SkyBox.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <SOIL/SOIL.h>
#include "./../Shader/Shader.h"
#include "./../Tools/Matrix/Matrix.h"
#include "./../Shader/Shader.cpp"
#include "./../Tools/Matrix/Matrix.cpp"
using namespace std;

SkyLayer::SkyLayer(const GLFWvidmode * mode) {
   this->mode = mode;
   this->program = CreateProgram(vertexSkyShader, fragmentSkyShader);
   this->setParameters();
   this->setVariablesLocation();
   this->setVariablesData();
};

void SkyLayer::cursorListener(GLFWwindow * window, double posX, double posY) {
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

void SkyLayer::listenContinouslyToCursor() {
   this->yaw += this->traceYaw/360.0f*2.0f;
   this->pitch += -this->tracePitch/360.0f*2.0f;
   if(this->yaw < 0)
       this->yaw += 2*M_PI;
   if(this->pitch < 0)
       this->pitch += 2*M_PI;
   this->yaw = fmod(this->yaw, 2*M_PI);
   this->pitch = fmod(this->pitch, 2*M_PI);
};

void SkyLayer::setParameters() {
   glDisable(GL_DEPTH_TEST);
   glDisable(GL_BLEND);
   glEnable(GL_TEXTURE_2D);
   glEnable(GL_CULL_FACE);
}

void SkyLayer::setVariablesLocation() {
   this->attribPositionLoc = glGetAttribLocation(program, "a_position");
   this->unifCameraLoc = glGetUniformLocation(program, "u_camera");
}

void SkyLayer::setVariablesData() {
   glUseProgram(this->program);

   vector<string> cycle = {
      "rt", "lf", "up", "bk", "bk", "ft"
   };

   GLuint texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
   
   for(int g = 0; g < 6; g++) {
      int width, height, channel;
      string file = "./../Tools/Textures/Sky/sky_" + cycle[g] + ".png";
      unsigned char * image = SOIL_load_image(file.c_str(), &width, &height, &channel, SOIL_LOAD_RGB);
      if(image == NULL) {
         cout << "Error while loading texture" << endl;
         exit(1);
      }

      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+g, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
      SOIL_free_image_data(image);
   }

   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

   glEnableVertexAttribArray(this->attribPositionLoc);

   // Buffer for position of Sky
   glGenBuffers(1, &this->bufferPosition);
   glBindBuffer(GL_ARRAY_BUFFER, this->bufferPosition);

   glBufferData(GL_ARRAY_BUFFER, pos.size()*sizeof(float), pos.data(), GL_STATIC_DRAW);
   glVertexAttribPointer(this->attribPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void SkyLayer::renderProgram() {
   glUseProgram(this->program);

   this->listenContinouslyToCursor();

   glBindBuffer(GL_ARRAY_BUFFER, this->bufferPosition);
   glVertexAttribPointer(this->attribPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

   this->cameraRotX.rotationX(this->pitch);
   this->cameraRotY.rotationY(this->yaw);
   this->cameraRotZ.rotationZ(this->roll);

   vector<float> quaternionRot = fromEuler(-this->pitch/M_PI*360.0f, this->yaw/M_PI*360, 0);
   quaternionMatrix.fromQuat(quaternionRot);

   this->cameraMatrix = this->cameraMatrix.inverseMatrix(this->quaternionMatrix);

   glUniformMatrix4fv(unifCameraLoc, 1, GL_FALSE, cameraMatrix.matrix.data());

   glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SkyLayer::freeProgram() {}