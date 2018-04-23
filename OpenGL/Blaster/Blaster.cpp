#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <list>
#include "./../Shot/Shot.h"
#include "./Blaster.h"

#include "./../Shot/Shot.cpp"
using namespace std;

Blaster::Blaster(const GLFWvidmode * mode) {
    list<Blaster> shots;
    this->mode = mode;
}

void Blaster::renderShots() {
	for(list<Shot>::iterator it = shots.begin(); it != shots.end();) {
		if(distanceVecs(it->transl, vector<float>{0, 0, 0}) > 30.0f)
			it = shots.erase(it);
		else {
			it->renderProgram();
			it++;
		}
	}
};

void Blaster::fireShots() {
	this->shots.push_back(Shot(this->mode, vector<float>{-0.5, -0.5, 0}, vector<float>{M_PI*3/2, M_PI*3/2, 0}));
	this->shots.push_back(Shot(this->mode, vector<float>{0.5, -0.5, 0}, vector<float>{M_PI*3/2, M_PI*3/2, 0}));
};