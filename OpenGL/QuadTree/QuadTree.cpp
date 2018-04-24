#include <iostream>
#include <vector>
#include <cmath>
#include "QuadTree.h"
#include "./../Box/Box.h"
#include "./../Box/Box.cpp"
#include "./../Tools/HeightmapReader/HeightmapReader.h"
#include "./../Tools/HeightmapReader/HeightmapReader.cpp"
#include "./../Tools/Matrix/Matrix.h"
#include "./../Tools/Matrix/Matrix.cpp"
#include "./../Node/Node.cpp"
#include "./../Node/Node.h"

using namespace std;

QuadTree::QuadTree() {
	readFile(this->mesh, "./../Tools/Heightmap/heightmap.jpg");

	this->cols = sqrt(this->mesh.size());
	this->rows = this->cols;

    this->depth = min(((int) floor(log2(this->cols))-1), 7);
    this->section = 1.0f/this->depth;

    this->tree = new Node(Box(0, 0, this->rows-1, this->cols-1));

	this->fillTree(this->tree, this->depth, 0);
}

QuadTree::~QuadTree() {
	delete this->tree;
}

void QuadTree::fillTree(Node * currentNode, int depth, int index) {
    if(depth == 0 || !currentNode->box.checkPartition())
        return;

    vector<int> vertixTriangleCords = currentNode->box.getVerticesCoord(this->cols);
    for(int g = 0; g < vertixTriangleCords.size(); g++) {
    	for(int h = 0; h < 3; h++) {
    		currentNode->vertices.push_back(this->mesh[vertixTriangleCords[g]][h]);
    	}
    }

    vector<int> vertixLineCords = currentNode->box.getLinesCoord(this->cols, index);
    for(int g = 0; g < vertixLineCords.size(); g++)
    	for(int h = 0; h < 3; h++) {
    		currentNode->lines.push_back(this->mesh[vertixLineCords[g]][h]);
    	}

    vector<Box> boxes = currentNode->box.getPartitions();
    for(int g = 0; g < 4; g++) {
    	currentNode->children[g] = new Node(boxes[g]);
    	this->fillTree(currentNode->children[g], depth-1, g);
    };
};

void QuadTree::checkFrustumBoundaries(vector<float> & vertices, Matrix & projection, Matrix & viewCamera) {
    float distance = -10.0f;
    bool withinFrustum = false;

    for(int g = 0; g < vertices.size(); g += 3) {

        vector<float> viewVector = viewCamera.multiplyVector(vector<float>{
                vertices[g],
                vertices[g+1],
                vertices[g+2],
                1.0
            }
        );

        vector<float> projectionVector = projection.multiplyVector(viewVector);

        for(int h = 0; h < 3; h++)
        	projectionVector[h] /= projectionVector[3];

        distance = fmax(distance, -distanceVecs(origin, viewVector));

        if(projectionVector[0] >= -1.0 && projectionVector[0] <= 1.0 &&
            projectionVector[1] >= -1.0 && projectionVector[1] <= 1.0 &&
            projectionVector[2] >= -1.0 && projectionVector[2] <= 1.0)
            withinFrustum = true;
    }

    this->boundary.distance = fmax(fabs(distance), 0)/10;
    this->boundary.withinFrustum = withinFrustum;
};

void QuadTree::readComplexity(vector<float> & data, Matrix & projection, Matrix & viewCamera, Node * currentNode, int index, int currentDepth) {
    this->checkFrustumBoundaries(currentNode->vertices, projection, viewCamera);

    if(this->boundary.withinFrustum) {
        int depth = this->depth-ceil((float) this->boundary.distance/this->section);

        if(depth <= currentDepth) {
            //Even complexity, or inherited from the parent previous complexity
            for(int g = 0; g < currentNode->vertices.size(); g++)
            	data.push_back(currentNode->vertices[g]);
        } else if(depth > currentDepth) {
            //Needs more complexity
            for(int g = 0; g < 4; g++)
            	this->readComplexity(data, projection, viewCamera, currentNode->children[g], g, currentDepth+1);
        }
    }
};

void QuadTree::readProjection(vector<float> & data, Matrix & projection, Matrix & viewCamera) {
	for(int g = 0; g < 4; g++)
	    this->readComplexity(data, projection, viewCamera, this->tree->children[g], g, 1);
};