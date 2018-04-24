#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include "./Shot.h"
#include "./../Shader/Shader.h"
#include "./../Tools/Matrix/Matrix.h"
// #include "./../Shader/Shader.cpp"
// #include "./../Tools/Matrix/Matrix.cpp"
using namespace std;

Shot::Shot(const GLFWvidmode * mode, vector<float> transl, vector<float> rotat) {
   this->program = CreateProgram(this->vertexShader, this->fragmentShader);
   this->mode = mode;
   this->transl = transl;
   this->rotat = rotat;

   srand(time(NULL));

   this->initializeShot();
   this->normalizeColor();

   setParameters();
   setVariablesLocation();
   setVariablesData();
};

void Shot::initializeShot() {
   for(int g = 0; g < this->nbParticles; g++) {
      vector<float> particle = this->initializeParticle();

      for(int h = 0; h < particle.size(); h++)
         this->particles.push_back(particle[h]);
   }
};

vector<float> Shot::initializeParticle() {
    float x = (float) rand()/RAND_MAX*this->tail-this->tail/2.0; //->[-this.tail, this.tail]

    return this->projectParticleProperties(x);
};

vector<float> Shot::projectParticleProperties(float x) {
    float max = this->getMaxBell(x);
    float y = ((float) rand())/RAND_MAX*(max+this->errorY*((float) rand())/RAND_MAX)-((max-this->errorY)/2.0);

    float rotationX = ((float) rand())/RAND_MAX*2*M_PI;

    Matrix rotation;
    rotation.rotationX(rotationX);

    vector<float> particle = rotation.multiplyVector(vector<float>{x, y, 0, 0});
    particle[3] = max;

    return particle;
};

float Shot::getMaxBell(float x) {
    float bellXNormalized = (x+this->tail/2.0); //->[0, this.tail]
    float bellXRange = bellXNormalized*this->range+1.0; //->[1.0, this.tail*this.range]

    return this->bell(bellXRange);
};

float Shot::bell(float x) {
    //Bell because the derivative of function is bell shape if Math.abs is used
    return sqrt(log(x))/pow(x, 6)*this->amplitude; //->D = [1, +inf]
};

void Shot::normalizeColor() {
   for(int g = 0; g < this->color.size(); g++)
      this->color[g] /= 255.0f;
};

void Shot::act() {
	this->transl = addValues(this->transl, this->direction);
};

void Shot::setParameters() {
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_PROGRAM_POINT_SIZE);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE);
};

void Shot::setVariablesLocation() {
   glUseProgram(this->program);

   this->attribPosLoc = glGetAttribLocation(this->program, "a_position");
   this->unifModelViewLoc = glGetUniformLocation(this->program, "u_model");
   this->unifProjectionLoc = glGetUniformLocation(this->program, "u_projection");
   this->unifColorLoc = glGetUniformLocation(this->program, "u_color");
   this->unifTailLoc = glGetUniformLocation(this->program, "u_tail");
   this->unifScalarTailNormalizerLoc = glGetUniformLocation(this->program, "u_scalarTailNormalizer");
};

void Shot::setVariablesData() {
   this->projection.perspective(M_PI/3, (float) this->mode->width/this->mode->height, 0.001, 30);

   glEnableVertexAttribArray(this->attribPosLoc);
   glGenBuffers(1, &this->posBuffer);
   glBindBuffer(GL_ARRAY_BUFFER, this->posBuffer);
   glBufferData(GL_ARRAY_BUFFER, this->particles.size()*sizeof(float), this->particles.data(), GL_DYNAMIC_DRAW);
   glVertexAttribPointer(this->attribPosLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
};

void Shot::renderProgram() {
   glUseProgram(this->program);
  
   this->translation.translation(this->transl[0], this->transl[1], this->transl[2]);

   vector<float> quat = fromEuler(this->rotat[0]/2/M_PI*360, this->rotat[1]/2/M_PI*360, this->rotat[2]/2/M_PI*360);
   this->modelView.fromQuat(quat);
   this->modelView = this->modelView*this->translation;

   glBindBuffer(GL_ARRAY_BUFFER, this->posBuffer);
   glBufferData(GL_ARRAY_BUFFER, this->particles.size()*sizeof(float), this->particles.data(), GL_DYNAMIC_DRAW);
   glVertexAttribPointer(this->attribPosLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);

   glUniformMatrix4fv(this->unifModelViewLoc, 1, GL_FALSE, this->modelView.matrix.data());
   glUniformMatrix4fv(this->unifProjectionLoc, 1, GL_FALSE, this->projection.matrix.data());
   glUniform3fv(this->unifColorLoc, 1, this->color.data());
   glUniform1f(this->unifTailLoc, this->tail);
   glUniform1f(this->unifScalarTailNormalizerLoc, 1/exp(-2.0f*this->tail));

   this->act();

   glDrawArrays(GL_POINTS, 0, this->nbParticles);
};

void Shot::freeProgram() {};