#include <iostream>
#include <cmath>
#include <vector>

#include "Box.h"
using namespace std;

Box::Box() {};

Box::Box(int x0, int y0, int x1, int y1) {
	this->x0 = x0;
	this->y0 = y0;
	this->x1 = x1;
	this->y1 = y1;
};


bool Box::checkPartition() {
    return ((this->x1-this->x0 >= 1) && (this->y1-this->y0 >= 1));
};


vector<Box> Box::getPartitions() {
    int diffX = this->x1-this->x0;
    int diffY = this->y1-this->y0;

    int partX = ceil((float) diffX/2.0f);
    int partY = ceil((float) diffY/2.0f);

    int xMax, yMax;
    vector<Box> boxes;
    for(int g = 0; g < 2; g++) {
        for(int h = 0; h < 2; h++) {
            xMax = min(this->x0+partX*(g+1), this->x1);
            yMax = min(this->y0+partY*(h+1), this->y1);
            boxes.push_back(Box(this->x0+partX*g, this->y0+partY*h, xMax, yMax));
        }
    }

    return boxes;
};

bool Box::checkBoundary(int tCols) {
    return (this->x0 >= 0 && this->x1 <= tCols-1 && this->y0 >= 0 && this->y1 <= tCols-1);
};

vector<int> Box::getVerticesCoord(int tCols) {
    return vector<int> {
        this->y0*tCols + this->x0,
        this->y1*tCols + this->x0,
        this->y1*tCols + this->x1,
        this->y1*tCols + this->x1,
        this->y0*tCols + this->x1,
        this->y0*tCols + this->x0,
    };
};

vector<int> Box::getLinesCoord(int tCols, int index) {
    if(index == 0 || index == 3)
        return vector<int>{
            this->y0*tCols + this->x0,
            this->y1*tCols + this->x0,
            this->y1*tCols + this->x0,
            this->y1*tCols + this->x1,
            this->y1*tCols + this->x1,
            this->y0*tCols + this->x0,
            this->y0*tCols + this->x0,
            this->y0*tCols + this->x1,
            this->y0*tCols + this->x1,
            this->y1*tCols + this->x1
        };
    else if(index == 1 || index == 2)
    	return vector<int>{
            this->y0*tCols + this->x1,
            this->y1*tCols + this->x1,
            this->y1*tCols + this->x1,
            this->y1*tCols + this->x0,
            this->y1*tCols + this->x0,
            this->y0*tCols + this->x1,
            this->y0*tCols + this->x1,
            this->y0*tCols + this->x0,
            this->y0*tCols + this->x0,
            this->y1*tCols + this->x0
        };
};

vector<Box> Box::getAdjacentBoxes(int tCols, int index) {
    int diffX = this->x1-this->x0;
    int diffY = this->y1-this->y0;

    if(index == 0) {
    	return vector<Box> {
    		Box(this->x0, max(this->y0-diffY*2, 0), this->x1+diffX, this->y0),
    		Box(max(this->x0-diffX*2, 0), this->y0, this->x0, this->y1+diffY)
    	};
    } else if(index == 1) {
    	return vector<Box> {
    		Box(max(this->x0-diffX*2, 0), min(this->y0-diffY, 0), this->x0, this->y1),
    		Box(this->x0, this->y1, min(this->x1+diffX, tCols-1), min(this->y1+diffY*2, tCols-1))
    	};
    } else if(index == 2) {
    	return vector<Box> {
    		Box(max(this->x0-diffX, 0), max(this->y0-diffY*2, 0), this->x1, this->y0),
    		Box(this->x1, this->y0, min(this->x1+diffX*2, tCols-1), min(this->y1+diffY, tCols-1))
    	};
    } else {
    	return vector<Box> {
    		Box(max(this->x0-diffX, 0), this->y1, this->x1, min(this->y1+diffY*2, tCols)),
    		Box(this->x1, max(this->y0-diffY, 0), min(this->x1+diffX*2, tCols), this->y1)
    	};
    }
};