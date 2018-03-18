#include "Sky.h"

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <math.h>
#include <cmath>
#include <assert.h>
#include <unistd.h>
#include <SOIL/SOIL.h>
// #include "./../Tools/Matrix/Matrix.cpp"
using namespace std;

unsigned int CompileShader(unsigned int type, const string& source) {
   unsigned int id = glCreateShader(type);
   const char * src = source.c_str();
   glShaderSource(id, 1, &src, NULL);
   glCompileShader(id);

   int result; 
   glGetShaderiv(id, GL_COMPILE_STATUS, &result);

   if(result == GL_FALSE) {
      cout << "Failed to compile shader" << std::endl;
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

int CreateProgram(const string& vertexShader, const string& fragmentShader) {
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


SkyLayer::SkyLayer() {
   rotationMX->rotationX(rotationX);
   rotationMY->rotationX(rotationY);
   rotationMZ->rotationX(rotationZ);
   translation->translation(0.0f, 0.0f, 0.0f);

   this->program = CreateProgram(vertexSkyShader, fragmentSkyShader);
   this->setDataLocations();
   this->setData();
};

void SkyLayer::setSettings() {
   // glEnable(GL_DEPTH_TEST);
   // glEnable(GL_CULL_FACE);
   // glDepthFunc(GL_LESS);
   // glEnable(GL_BLEND);
   // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SkyLayer::setDataLocations() {
   vertexPositionLocation = glGetAttribLocation(program, "a_position");
   matrixCameraLocation = glGetUniformLocation(program, "u_camera");
}

void SkyLayer::setData() {
   vector<string> texts = {
      "rt", "lf", "up", "dn", "bk", "ft"
   };

   GLuint texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
   
   for(int g = 0; g < 6; g++) {
      int width, height;
      string file = "./../Tools/Textures/World/lagoon_";
      file = file + texts[g] + ".jpg";
      unsigned char * image = SOIL_load_image(file.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
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

   vector<float> pos = {
      -1.0f,  -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
      1.0f, 1.0f, -1.0f, -1.0f,  -1.0f, -1.0f, 1.0f, -1.0f, -1.0f
   };

   // Buffer for position of Sky
   glGenBuffers(1, &this->bufferPosition);
   glBindBuffer(GL_ARRAY_BUFFER, this->bufferPosition);

   glBufferData(GL_ARRAY_BUFFER, pos.size()*sizeof(float), pos.data(), GL_STATIC_DRAW);
   glEnableVertexAttribArray(vertexPositionLocation);
   glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

void SkyLayer::renderSky() {
   glUseProgram(program);

   rotationX += offsetX;
   rotationMX->rotationX(rotationX);

   rotationY += offsetY;
   rotationMY->rotationY(rotationY);

   u_swap1->multiplyMatrices(rotationMY, rotationMX);
   u_swap2->multiplyMatrices(u_swap1, rotationMZ);
   u_swap1->multiplyMatrices(u_swap2, translation);

   u_swap2->inverseMatrix(u_swap1);

   glUniformMatrix4fv(matrixCameraLocation, 1, GL_FALSE, u_swap2->matrix);

   glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SkyLayer::freeSky() {
   delete u_swap1;
   delete u_swap2;
   delete rotationMX;
   delete rotationMY;
   delete rotationMZ;
   delete translation;
}