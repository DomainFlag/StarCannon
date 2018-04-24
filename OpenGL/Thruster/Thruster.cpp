#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include "./Thruster.h"
#include "./../Shader/Shader.h"
#include "./../Tools/Matrix/Matrix.h"

// #include "./../Shader/Shader.cpp"
// #include "./../Tools/Matrix/Matrix.cpp"
using namespace std;

Thruster::Thruster(const GLFWvidmode * mode) {
   this->program = CreateProgram(this->vertexShader, this->fragmentShader);
   this->mode = mode;

   srand(time(NULL));

   this->initializeThruster();

   setParameters();
   setVariablesLocation();
   setVariablesData();
};

vector<float> Thruster::projectParticleProperties(float x) {
    float max = this->getMaxBell(x);
    float y = ((float) rand())/RAND_MAX*(max+this->errorY*((float) rand())/RAND_MAX)-((max-this->errorY)/2.0);

    float rotationX = ((float) rand())/RAND_MAX*2*M_PI;

    Matrix rotation;
    rotation.rotationX(rotationX);

    vector<float> particle = rotation.multiplyVector(vector<float>{x, y, 0, 0});
    particle[3] = max;

    return particle;
};

vector<float> Thruster::initializeParticle() {
    float x = (float) rand()/RAND_MAX*this->tail-this->tail/2.0; //->[-this.tail, this.tail]

    return this->projectParticleProperties(x);
};


void Thruster::initializeThruster() {
   for(int g = 0; g < this->nbParticles; g++) {
      vector<float> particle = this->initializeParticle();

      for(int h = 0; h < particle.size(); h++)
         this->particles.push_back(particle[h]);
   }

   this->normalizeColor();
};

// void Thruster::addEventListener = function() {
//     document.addEventListener("keypress", function(e) {
//         if(e.which === 119) {
//             if(thruster.speed < this.maxSpeed-this.stepSpeed)
//                 thruster.speed += this.stepSpeed;
//         } else if(e.which === 115) {
//             if(thruster.speed > this.minSpeed+this.stepSpeed)
//                 thruster.speed -= this.stepSpeed;
//         } else return;

//         thruster.errorX = this.lerp();
//     }.bind(this));
// };

void Thruster::normalizeColor() {
   for(int g = 0; g < this->color.size(); g++)
      this->color[g] /= 255.0f;
};

float Thruster::lerp() {
    return this->minSpeed+(this->speed-this->minSpeed)/(this->maxSpeed-this->minSpeed)*(this->tail-0);
};

vector<float> Thruster::recycle() {
    float x = -this->tail/2.0+((float) rand())/RAND_MAX*this->errorX; //->[-this.tail+err, -this.tail+err]

    return this->projectParticleProperties(x);
};

float Thruster::bell(float x) {
    //Bell because the derivative of function is bell shape if Math.abs is used
    return sqrt(log(x))/pow(x, 6)*this->amplitude; //->D = [1, +inf]
};

float Thruster::getMaxBell(float x) {
    float bellXNormalized = (x+this->tail/2.0); //->[0, this.tail]
    float bellXRange = bellXNormalized*this->range+1.0; //->[1.0, this.tail*this.range]

    return this->bell(bellXRange);
};

void Thruster::act() {
    for(int g = 0; g < this->particles.size(); g += 4) {
        if(this->particles[g] > this->tail/2.0 ||
            abs(this->particles[g+1]) > this->particles[g+3] ||
            abs(this->particles[g+2]) > this->particles[g+3]) {
            vector<float> recycledParticles = this->recycle();
            for(int h = 0; h < recycledParticles.size(); h++)
               this->particles[g+h] = recycledParticles[h];
         } else {
            float proportionY = this->particles[g+1]/this->particles[g+3];
            float proportionZ = this->particles[g+2]/this->particles[g+3];

            this->particles[g] += this->speed*((float) rand())/RAND_MAX;

            float max = this->getMaxBell(this->particles[g]);
            this->particles[g+1] = proportionY*max;
            this->particles[g+2] = proportionZ*max;
            this->particles[g+3] = max;
        }
    }
};

void Thruster::setParameters() {
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_PROGRAM_POINT_SIZE);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   glEnable(GL_CULL_FACE);
};

void Thruster::setVariablesLocation() {
   glUseProgram(this->program);

   this->attribPosLoc = glGetAttribLocation(this->program, "a_position");
   this->unifModelViewLoc = glGetUniformLocation(this->program, "u_model");
   this->uniProjectionLoc = glGetUniformLocation(this->program, "u_projection");
   this->unifColorLoc = glGetUniformLocation(this->program, "u_color");
   this->unifTailLoc = glGetUniformLocation(this->program, "u_tail");
   this->unifScalarTailNormalizerLoc = glGetUniformLocation(this->program, "u_scalarTailNormalizer");
};

void Thruster::setVariablesData() {
   this->translation.translation(this->transl[0], this->transl[1], this->transl[2]);

   this->projection.perspective(M_PI/3.0f, this->mode->width/this->mode->height, 0.001, 30);

   glEnableVertexAttribArray(this->attribPosLoc);
   glGenBuffers(1, &this->posBuffer);
   glBindBuffer(GL_ARRAY_BUFFER, this->posBuffer);
   glBufferData(GL_ARRAY_BUFFER, this->particles.size()*sizeof(float), this->particles.data(), GL_DYNAMIC_DRAW);
   glVertexAttribPointer(this->attribPosLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
};

void Thruster::renderProgram() {
	this->setParameters();
	glUseProgram(this->program);

	this->act();

	vector<float> quat = fromEuler(this->rotationX/2/M_PI*360, this->rotationY/2/M_PI*360, this->rotationZ/2/M_PI*360);
	this->modelView.fromQuat(quat);
	this->modelView = this->modelView*this->translation;

	glBindBuffer(GL_ARRAY_BUFFER, this->posBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->particles.size()*sizeof(float), this->particles.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(this->attribPosLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);

	glUniformMatrix4fv(this->unifModelViewLoc, 1, GL_FALSE, this->modelView.matrix.data());
	glUniformMatrix4fv(this->uniProjectionLoc, 1, GL_FALSE, this->projection.matrix.data());
	glUniform3fv(this->unifColorLoc, 1, this->color.data());
	glUniform1f(this->unifTailLoc, this->tail);
	glUniform1f(this->unifScalarTailNormalizerLoc, 1/exp(-2.0f*this->tail));

	glDrawArrays(GL_POINTS, 0, this->nbParticles);
};

void Thruster::freeProgram() {};