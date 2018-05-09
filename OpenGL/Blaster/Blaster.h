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

    vector<float> direction{0, 0, -0.25};
    vector<float> position{0, -0.5, -1.5};

    Blaster(const GLFWvidmode * mode);

    void renderShots();

    void fireShots();
};

#endif