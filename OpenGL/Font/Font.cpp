#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;

FT_Face face;
FT_GlyphSlot g;

unsigned int CompileShader(unsigned int type, const string & source) {
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

int CreateProgram(const string & vertexShader, const string & fragmentShader) {
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

void render_text(const char *text, float x, float y, float sx, float sy) {
	const char *p;

	for(p = text; *p; p++) {
		if(FT_Load_Char(face, *p, FT_LOAD_RENDER))
		    continue;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, g->bitmap.width, g->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

		float x2 = x + g->bitmap_left * sx;
		float y2 = -y - g->bitmap_top * sy;
		float w = g->bitmap.width * sx;
		float h = g->bitmap.rows * sy;

		GLfloat box[4][4] = {
		    {x2,     -y2    , 0, 0},
		    {x2 + w, -y2    , 1, 0},
		    {x2,     -y2 - h, 0, 1},
		    {x2 + w, -y2 - h, 1, 1},
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		x += (g->advance.x/64) * sx;
		y += (g->advance.y/64) * sy;
	}
}

int main() {
	GLFWwindow * window;

	if(!glfwInit())
	   return -1;

	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	window = glfwCreateWindow(mode->width, mode->height, "Text rendering", glfwGetPrimaryMonitor(), NULL);

	if(!window) {
	   glfwTerminate();
	   return -1;
	}

	glfwMakeContextCurrent(window);

	if(glewInit() != GLEW_OK) {
	   cout << "error..!!" << endl;
	}

	cout << glGetString(GL_VERSION) << endl;

	/* FreeType */

	FT_Library ft;

	if(FT_Init_FreeType(&ft)) {
	  fprintf(stderr, "Could not init freetype library\n");
	  return 1;
	}


	if(FT_New_Face(ft, "./Roboto/Roboto-Regular.ttf", 0, &face)) {
	  fprintf(stderr, "Could not open font\n");
	  return 1;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	if(FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
	  fprintf(stderr, "Could not load character 'X'\n");
	  return 1;
	}

	g = face->glyph;

	string vertexShader = R"(
		#version 130

		attribute vec4 coord;
		varying vec2 texcoord;

		void main(void) {
		  gl_Position = vec4(coord.xy, 0, 1);
		  texcoord = coord.zw;
		}
	)";

	string fragmentShader = R"(
		#version 130

		varying vec2 texcoord;
		uniform sampler2D tex;
		uniform vec4 color;

		void main(void) {
		  gl_FragColor = vec4(1, 1, 1, texture2D(tex, texcoord).r) * color;
		}
	)";

	GLuint program = CreateProgram(vertexShader, fragmentShader);
	glUseProgram(program);

	/* FreeType End */

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLint attribute_coord = glGetAttribLocation(program, "coord");
	GLint uniform_color = glGetUniformLocation(program, "color");
	GLint uniform_tex = glGetUniformLocation(program, "tex");

	GLuint tex;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(uniform_tex, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glEnableVertexAttribArray(attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glViewport(0, 0, mode->width, mode->height);

    GLfloat black[4] = {0, 0, 0, 1};
    glUniform4fv(uniform_color, 1, black);

    float sx = 2.0 / mode->width;
    float sy = 2.0 / mode->height;

	while(!glfwWindowShouldClose(window)) {
	   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	   render_text("Text here...?",
	               -1 + 8 * sx,   1 - 50 * sy,    sx, sy);
	   render_text("Easy one. :)",
	               -1 + 8.5 * sx, 1 - 105.5 * sy, sx, sy);

	   glfwSwapBuffers(window);
	   glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
