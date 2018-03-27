#ifndef SHADER
#define SHADER

#include <string>
using namespace std;

unsigned int CompileShader(unsigned int type, const string & source);

int CreateProgram(const string & vertexShader, const string & fragmentShader);

#endif