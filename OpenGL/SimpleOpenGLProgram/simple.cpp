#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <assert.h>
#include <unistd.h>
#include <SOIL/SOIL.h> 
#include "./../Tools/Matrix/matrix.cpp"
using namespace std;

GLuint prog_hdlr;
GLint	a_position;

const GLFWvidmode * mode;

Matrix * translation = new Matrix();

float rotationX = 0.0f;
float rotationY = 0.0f;
float rotationZ = 0.0f;

Matrix * rotationMX = new Matrix();
Matrix * rotationMY = new Matrix();
Matrix * rotationMZ = new Matrix();

static unsigned int CompileShader(unsigned int type, const string& source) {
   unsigned int id = glCreateShader(type);
   cout << source.c_str() << endl;
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
      case GLFW_KEY_Q : {
         rotationX += 0.05;
         rotationMX->rotationX(rotationX);
         break;
      };
      case GLFW_KEY_W : {
         rotationY += 0.05;
         rotationMY->rotationY(rotationY);
         break;
      };
      case GLFW_KEY_A : {
         rotationX -= 0.05;
         rotationMX->rotationX(rotationX);
         break;
      };
      case GLFW_KEY_S : {
         rotationY -= 0.05;
         rotationMY->rotationY(rotationY);
         break;
      };
      case GLFW_KEY_D : {
         break;
      };
   };
}

static void cursor(GLFWwindow * window, double x, double y) {
   double unitX = (2*x-mode->width)/mode->width;
   double unitY = (2*y-mode->height)/mode->height;

   float translationX = ((float) unitX)/5.0;
   float translationY = -((float) unitY)/5.0;

   delete translation;
   translation = new Matrix();
   translation->translation(translationX, translationY, 0.0f);
}

int main(int argc, char ** argv) {
   GLFWwindow * window;

   cout << glGetString(GL_VERSION) << endl;

   if(!glfwInit())
      return -1;

   mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

   window = glfwCreateWindow(mode->width, mode->height, "Textured rectangle rendering", glfwGetPrimaryMonitor(), NULL);
   if(!window) {
      glfwTerminate();
      return -1;
   }

   glfwMakeContextCurrent(window);

   if(glewInit() != GLEW_OK) {
      std::cout << "error..!!" << std::endl;
   }

   translation->translation(0.0f, 0.0f, 0.0f);
   rotationMX->rotationX(rotationX);
   rotationMY->rotationX(rotationY);
   rotationMZ->rotationX(rotationZ);

   float positions[108] = {
      -0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, -0.5f, 0.5f, 0.0f,
      0.5f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f,

      -0.5f, -0.5f, -1.0f, -0.5f, 0.5f, -1.0f, 0.5f, 0.5f, -1.0f,
      0.5f, 0.5f, -1.0f, 0.5f, -0.5f, -1.0f, -0.5f, -0.5f, -1.0f,

      -0.5f, -0.5f, 0.0f, -0.5f, 0.5f, 0.0f, -0.5f, 0.5f, -1.0f,
      -0.5f, 0.5f, -1.0f, -0.5f, -0.5f, -1.0f, -0.5f, -0.5f, 0.0f,

      0.5f, -0.5f, 0.0f, 0.5f, 0.5f, -1.0f, 0.5f, 0.5f, 0.0f,
      0.5f, 0.5f, -1.0f, 0.5f, -0.5f, 0.0f, 0.5f, -0.5f, -1.0f,

      -0.5f, 0.5f, 0.0f, 0.5f, 0.5f, -1.0f, -0.5f, 0.5f, -1.0f,
      0.5f, 0.5f, -1.0f, -0.5f, 0.5f, 0.0f, 0.5f, 0.5f, 0.0f, 

      -0.5f, -0.5f, 0.0f, -0.5f, -0.5f, -1.0f, 0.5f, -0.5f, -1.0f,
      0.5f, -0.5f, -1.0f, 0.5f, -0.5f, 0.0f, -0.5f, -0.5f, 0.0f
   };

   float colors[108] = {
      1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,

      0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,  1.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,  1.0f, 1.0f, 0.0f,

      0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
      0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,  1.0f, 1.0f,

      1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
   };

   string vertexShader = R"(
      #version 130
      attribute vec4 a_position;
      attribute vec3 a_color;
      varying vec3 v_color;
      uniform mat4 u_matrix;
      varying vec4 v_position;
      void main() {
         mat4 a_matrix;
         a_matrix = mat4(1.0f);
         vec4 pos = u_matrix*a_position;
         gl_Position = vec4(pos.xyz, 1.0);
         v_color = a_color;
         v_position = a_position;
      }
   )";

   string fragmentShader = R"(
      #version 130
      precision mediump float;
      uniform sampler2D u_texture;
      varying vec3 v_color;
      varying vec4 v_position;
      void main() {
         gl_FragColor = texture2D(u_texture, (v_position.xy+1.0)/2.0);
      }
   )";


   unsigned int program = CreateProgram(vertexShader, fragmentShader);
   glUseProgram(program);

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

   glfwSetKeyCallback(window, input);
   glfwSetCursorPosCallback(window, cursor);

   GLint attributePositionLocation = glGetAttribLocation(program, "a_position");
   GLint uniformMatrixLocation = glGetUniformLocation(program, "u_matrix");
   GLint attributeColorLocation = glGetAttribLocation(program, "a_color");

   unsigned int buffer;
   glGenBuffers(1, &buffer);
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   glBufferData(GL_ARRAY_BUFFER, 108*sizeof(float), positions, GL_STATIC_DRAW);

   glEnableVertexAttribArray(attributePositionLocation);
   glVertexAttribPointer(attributePositionLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);

   unsigned int bufferColor;
   glGenBuffers(1, &bufferColor);
   glBindBuffer(GL_ARRAY_BUFFER, bufferColor);
   glBufferData(GL_ARRAY_BUFFER, 108*sizeof(float), colors, GL_STATIC_DRAW);

   glEnableVertexAttribArray(attributeColorLocation);
   glVertexAttribPointer(attributeColorLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);


   GLuint texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
   int width, height;
   unsigned char * image = SOIL_load_image("./../Tools/textures/water1.jpg", &width, &height, 0, SOIL_LOAD_RGB);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
   glGenerateMipmap(GL_TEXTURE_2D);
   SOIL_free_image_data(image);

   Matrix * u_swap1 = new Matrix();
   Matrix * u_swap2 = new Matrix();

   while(!glfwWindowShouldClose(window)) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      u_swap1->multiplyMatrices(rotationMX, rotationMY);
      u_swap2->multiplyMatrices(u_swap1, rotationMZ);
      u_swap1->multiplyMatrices(u_swap2, translation);

      glUniformMatrix4fv(uniformMatrixLocation, 1, GL_FALSE, u_swap1->matrix);
      glDrawArrays(GL_TRIANGLES, 0, 36);
      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   delete u_swap1, u_swap2;
   delete translation, rotationMX, rotationMY, rotationMZ;

   glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

   glfwTerminate();
   return 0;
}