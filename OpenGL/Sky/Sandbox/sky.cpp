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
#include "./../Tools/Matrix/matrix.cpp"
using namespace std;

GLuint prog_hdlr;
GLint a_position;

const GLFWvidmode * mode;

Matrix * rotationMX = new Matrix();
Matrix * rotationMY = new Matrix();
Matrix * rotationMZ = new Matrix();
float rotationX = 0.0f;
float rotationY = 0.0f;
float rotationZ = 0.0f;

Matrix * translation = new Matrix();
float translationZ = 0.0f;

double offsetX = 0.0f;
double offsetY = 0.0f; 


static unsigned int CompileShader(unsigned int type, const string& source) {
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
         if(translationZ < 1.0f)
             translationZ += 0.05f;
          translation->translation(0.0f, 0.0f, translationZ);
         break;
      };
      case GLFW_KEY_S : {
         if(translationZ > -1.0f)
             translationZ -= 0.05f;
          translation->translation(0.0f, 0.0f, translationZ);
         break;
      };
      case GLFW_KEY_D : {
         //... strafing translationX pos with rotationZ neg
         break;
      };
   };
}

void cursor(GLFWwindow * window, double x, double y) {
   double variation = 1/50.0;
   offsetY = (x-mode->width/2.0)/(mode->width/2.0)*variation;
   offsetX = (y-mode->height/2.0)/(mode->height/2.0)*variation;
}

int main(int argc, char ** argv) {
   GLFWwindow * window;

   if(!glfwInit())
      return -1;

   mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

   window = glfwCreateWindow(mode->width, mode->height, "Skybox rendering", glfwGetPrimaryMonitor(), NULL);
   if(!window) {
      glfwTerminate();
      return -1;
   }

   glfwMakeContextCurrent(window);

   if(glewInit() != GLEW_OK) {
      std::cout << "error..!!" << std::endl;
   }

   cout << glGetString(GL_VERSION) << endl;

   rotationMX->rotationX(rotationX);
   rotationMY->rotationX(rotationY);
   rotationMZ->rotationX(rotationZ);
   translation->translation(0.0f, 0.0f, 0.0f);

   string vertexSkyShader = R"(
      #version 130
      attribute vec4 a_position;
      uniform mat4 u_camera;
      varying vec4 v_texture;

      void main() {
         gl_Position = vec4(a_position.xyz, 1.0);
         v_texture = u_camera*a_position;
      }
   )";

   string fragmentSkyShader = R"(
      #version 130
      precision mediump float;
      varying vec4 v_texture;
      uniform samplerCube u_skybox;

      void main() {
         gl_FragColor = textureCube(u_skybox, v_texture.xyz);
      } 
   )";

   unsigned int programSky = CreateProgram(vertexSkyShader, fragmentSkyShader);
   glUseProgram(programSky);

   glViewport(0, 0, mode->width, mode->height);

   glEnable(GL_CULL_FACE);
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

   glfwSetKeyCallback(window, input);
   glfwSetCursorPosCallback(window, cursor);

   GLint vertexPositionLocation = glGetAttribLocation(programSky, "a_position");
   GLint matrixCameraLocation = glGetUniformLocation(programSky, "u_camera");

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
   unsigned int bufferPosition;
   glGenBuffers(1, &bufferPosition);
   glBindBuffer(GL_ARRAY_BUFFER, bufferPosition);

   glBufferData(GL_ARRAY_BUFFER, pos.size()*sizeof(float), pos.data(), GL_STATIC_DRAW);
   glEnableVertexAttribArray(vertexPositionLocation);
   glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);

   Matrix * u_swap1 = new Matrix();
   Matrix * u_swap2 = new Matrix();

   while(!glfwWindowShouldClose(window)) {
      glClear(GL_COLOR_BUFFER_BIT);

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
      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   delete u_swap1;
   delete u_swap2;
   delete rotationMX;
   delete rotationMY;
   delete rotationMZ;
   delete translation;

   glfwTerminate();
   return 0;
}