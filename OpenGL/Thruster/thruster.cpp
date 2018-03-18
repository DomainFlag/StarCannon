#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <SOIL/SOIL.h> 
#include "./../Tools/Matrix/Matrix.h"
using namespace std;

GLuint prog_hdlr;
GLint	a_position;

const GLFWvidmode * mode;

float speedScalar = 0.5f;

Matrix * translation = new Matrix();
float translationZ = 0.0f;

Matrix * rotationMX = new Matrix();
Matrix * rotationMY = new Matrix();
Matrix * rotationMZ = new Matrix();
float rotationX = 0.0f;
float rotationY = 0.0f;
float rotationZ = 0.0f;

class Thruster {
public:
   float tail = -0.6f;

   int nbParticles = 15000;
   float bellfat = 15.0;

   vector<float> particles;

   Thruster() {
      float x, y, max;
      for(int g = 0; g < this->nbParticles; g++) {
         x = (float) rand()/RAND_MAX*this->tail;
         max = this->bell(x);
         y = (float) rand()/RAND_MAX*max;

         float rotation = (float) rand()/RAND_MAX*2*M_PI;
         Matrix * rotationM = new Matrix();
         rotationM->rotationX(rotation);

         vector<float> data = {0, 0, 0, 0};
         rotationM->multiplyVector(vector<float>{x, y, 0, 0}, data);
         for(int g = 0; g < data.size()-1; g++){
            this->particles.push_back(data[g]);
         }

         delete rotationM;
      }
   };

   float bell(float x) {
      return ((-exp(x*x)+1.0f)/(exp(1.0f)-1.0f)+1.0f)/this->bellfat;
   };

   void act() {
     for(int g = 0; g < this->nbParticles*3; g += 3) {
       if(this->particles[g] > -(this->tail+1.0)) {
         this->particles[g] = this->tail+speedScalar*(float)rand()/RAND_MAX*3.0f;
         float max = this->bell(this->particles[g]);
         this->particles[g+1] = (float)rand()/RAND_MAX*max;

         float rotation = (float)rand()/RAND_MAX*2*M_PI;
         Matrix * rotationM = new Matrix();
         rotationM->rotationX(rotation);

         vector<float> data = {0, 0, 0, 0};
         rotationM->multiplyVector(vector<float>{this->particles[g], this->particles[g+1], 0, 0}, data);

         this->particles[g] = data[0];
         this->particles[g+1] = data[1];
         this->particles[g+2] = data[2];
         delete rotationM;
      } else {
            float proportionY = this->particles[g+1]/this->bell(this->particles[g]);
            float proportionZ = this->particles[g+2]/this->bell(this->particles[g]);
            this->particles[g] += speedScalar*(float)rand()/RAND_MAX;
            this->particles[g+1] = proportionY*this->bell(this->particles[g]);
            this->particles[g+2] = proportionZ*this->bell(this->particles[g]);
         }
      }
   };
};

void setCube(vector<float> & cube, float x1, float y1, float length) {
   cube = {
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
}

void setColors(vector<float> & palette) {
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
         palette.push_back(colors[g]);
         palette.push_back(colors[g+1]);
         palette.push_back(colors[g+2]);
      }
   }
}


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

void input(GLFWwindow * window, int key, int action, int u, int i) {
   switch(key) {
      case GLFW_KEY_A : {
         //... strafing translationX neg with rotationZ pos
         break;
      };
      case GLFW_KEY_W : {
         if(speedScalar < 1.0f)
             speedScalar += 0.04f;
         break;
      };
      case GLFW_KEY_S : {
         if(speedScalar > 0.05f)
             speedScalar -= 0.04;
         break;
      };
      case GLFW_KEY_D : {
         //... strafing translationX pos with rotationZ neg
         break;
      };

      case GLFW_KEY_R : {
         translationZ += 0.1;
         translation->translation(0.0f, 0.0f, translationZ);
         break;
      };
      case GLFW_KEY_F : {
         translationZ -= 0.1;
         translation->translation(0.0f, 0.0f, translationZ);
         break;
      };

      case GLFW_KEY_T : {
         rotationX += 0.1;
         rotationMX->rotationX(rotationX);
         break;
      };
      case GLFW_KEY_G : {
         rotationX -= 0.1;
         rotationMX->rotationX(rotationX);
         break;
      };


      case GLFW_KEY_Y : {
         rotationY += 0.1;
         rotationMY->rotationY(rotationY);
         break;
      };
      case GLFW_KEY_H : {
         rotationY -= 0.1;
         rotationMY->rotationY(rotationY);
         break;
      };


      case GLFW_KEY_U : {
         rotationZ += 0.1;
         rotationMZ->rotationY(rotationZ);
         break;
      };
      case GLFW_KEY_J : {
         rotationZ -= 0.1;
         rotationMZ->rotationY(rotationZ);
         break;
      };
   };
}

static void cursor(GLFWwindow * window, double x, double y) {

   double unitX = (2*x-mode->width)/mode->width;
   double unitY = (2*y-mode->height)/mode->height;

   float translationX = ((float) unitX)/5.0;
   float translationY = -((float) unitY)/5.0;

   translation->matrix[12] = translationY;
   translation->matrix[13] = translationY;
}

int main(int argc, char ** argv) {
   GLFWwindow * window;

   if(!glfwInit())
      return -1;

   mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

   window = glfwCreateWindow(mode->width, mode->height, "Thruster rendering", glfwGetPrimaryMonitor(), NULL);
   if(!window) {
      glfwTerminate();
      return -1;
   }

   glfwMakeContextCurrent(window);

   if(glewInit() != GLEW_OK) {
      std::cout << "error..!!" << std::endl;
   }

   cout << glGetString(GL_VERSION) << endl;

   translation->translation(0.0f, 0.0f, 0.0f);
   rotationMX->rotationX(rotationX);
   rotationMY->rotationX(rotationY);
   rotationMZ->rotationX(rotationZ);

   Thruster thruster;

   string vertexThrusterShader = R"(
      #version 130
   attribute vec4 a_position;
   varying float v_color;
   uniform mat4 u_matrix;
   uniform mat4 u_matrixO;
   void main() {
     gl_PointSize = 3.5;
     vec4 pos = u_matrix*a_position;
     gl_Position = vec4(pos.xyz, (pos.z+1.0));
     v_color = pos.z+1.0;
   }
   )";

   string fragmentThrusterShader = R"(
      #version 130
   precision mediump float;
   varying float v_color;
   void main() {
     gl_FragColor = vec4(1, 0.2, 0, v_color);
   }
   )";

   string vertexCubeShader = R"(
      #version 130
      attribute vec4 a_position;
      uniform mat4 u_matrix;
      uniform mat4 u_matrixO;
      attribute vec4 a_color;
      varying vec4 v_color;
      void main() {
        vec4 pos = u_matrix*a_position;
        gl_Position = vec4(pos.xyz, (pos.z+1.0));
        v_color = a_color;
      }
   )";

   string fragmentCubeShader = R"(
      #version 130
      precision mediump float;
      varying vec4 v_color;
      void main() {
        gl_FragColor = v_color;
      }
   )";

   unsigned int programThruster = CreateProgram(vertexThrusterShader, fragmentThrusterShader);
   glUseProgram(programThruster);

   unsigned int programCube = CreateProgram(vertexCubeShader, fragmentCubeShader);
   glUseProgram(programCube);

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glEnable(GL_PROGRAM_POINT_SIZE);
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

   glfwSetKeyCallback(window, input);
   glfwSetCursorPosCallback(window, cursor);

   GLint attributeThrusterPositionLocation = glGetAttribLocation(programThruster, "a_position");
   GLint matrixThrusterPositionLocation = glGetUniformLocation(programThruster, "u_matrix");
   GLint matrixThrusterOLocation = glGetUniformLocation(programThruster, "u_matrixO");


   GLint attributeObjectPositionLocation = glGetAttribLocation(programCube, "a_position");
   GLint matrixObjectPositionLocation = glGetUniformLocation(programCube, "u_matrix");
   GLint attributeObjectColorLocation = glGetAttribLocation(programCube, "a_color");
   GLint matrixObjectOLocation = glGetUniformLocation(programCube, "u_matrixO");

   glUseProgram(programThruster);

   // Buffer for position of thruster
   unsigned int bufferThrusterPosition;
   glGenBuffers(1, &bufferThrusterPosition);
   glBindBuffer(GL_ARRAY_BUFFER, bufferThrusterPosition);

   glEnableVertexAttribArray(attributeThrusterPositionLocation);

   glUseProgram(programCube);

   // Buffer for positionof object with the thruster
   unsigned int bufferObjectPosition;
   glGenBuffers(1, &bufferObjectPosition);
   glBindBuffer(GL_ARRAY_BUFFER, bufferObjectPosition);

   glEnableVertexAttribArray(attributeObjectPositionLocation);

   vector<float> cube;
   setCube(cube, -1.0f, -0.2f, 0.4f);
   glBufferData(GL_ARRAY_BUFFER, 108*sizeof(float), cube.data(), GL_STATIC_DRAW);
   glVertexAttribPointer(attributeObjectPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);

   // Buffer for colors for object with the thruster
   unsigned int bufferObjectColor;
   glGenBuffers(1, &bufferObjectColor);
   glBindBuffer(GL_ARRAY_BUFFER, bufferObjectColor);

   vector<float> colors; 
   setColors(colors);
   glBufferData(GL_ARRAY_BUFFER, 108*sizeof(float), colors.data(), GL_STATIC_DRAW);

   glEnableVertexAttribArray(attributeObjectColorLocation);
   glVertexAttribPointer(attributeObjectColorLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);

   glBindBuffer(GL_ARRAY_BUFFER, bufferThrusterPosition);

   Matrix * u_swap1 = new Matrix();
   Matrix * u_swap2 = new Matrix();

   while(!glfwWindowShouldClose(window)) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      u_swap1->multiplyMatrices(rotationMX, rotationMY);
      u_swap2->multiplyMatrices(u_swap1, rotationMZ);
      u_swap1->multiplyMatrices(u_swap2, translation);

      u_swap2->inverseMatrix(u_swap1);

      glUseProgram(programCube);
      // glEnableVertexAttribArray(attributeObjectPositionLocation);

      glBindBuffer(GL_ARRAY_BUFFER, bufferObjectPosition);
      glUniformMatrix4fv(matrixObjectPositionLocation, 1, GL_FALSE, u_swap2->matrix);
      glVertexAttribPointer(attributeObjectPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);

      glDrawArrays(GL_TRIANGLES, 0, 36);
      // glDisableVertexAttribArray(attributeObjectPositionLocation);

      glUseProgram(programThruster);
      // glEnableVertexAttribArray(attributeThrusterPositionLocation);

      glBindBuffer(GL_ARRAY_BUFFER, bufferThrusterPosition);
      glBufferData(GL_ARRAY_BUFFER, thruster.particles.size()*sizeof(float), thruster.particles.data(), GL_STATIC_DRAW);
      glVertexAttribPointer(attributeThrusterPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
      thruster.act();

      glUniformMatrix4fv(matrixThrusterPositionLocation, 1, GL_FALSE, u_swap2->matrix);

      glDrawArrays(GL_POINTS, 0, (int)thruster.particles.size()/3.0f);
      // glDisableVertexAttribArray(attributeThrusterPositionLocation);

      glfwSwapBuffers(window);

      glfwPollEvents();
   }

   delete u_swap1;
   delete u_swap2;
   delete translation;
   delete rotationMX;
   delete rotationMY;
   delete rotationMZ;

   glfwTerminate();
   return 0;
}