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

using namespace std;

Blaster::Blaster(const GLFWvidmode * mode) {
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
    this->position[0] -= 0.125;
	this->shots.push_back(Shot(this->mode, this->direction, this->position, vector<float>{M_PI*3/2, M_PI*3/2, 0}));
    this->position[0] += 0.25;
	this->shots.push_back(Shot(this->mode, this->direction, this->position, vector<float>{M_PI*3/2, M_PI*3/2, 0}));
    this->position[0] -= 0.125;
};