#ifndef BLASTER
#define BLASTER

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <list>
#include "./../Tools/Matrix/Matrix.h"
#include "./../Shot/Shot.h"
using namespace std;


class Blaster {
public:
    const GLFWvidmode * mode;

    list<Shot> shots;

    Blaster(const GLFWvidmode * mode);

    void renderShots();

    void fireShots();
};

#endif