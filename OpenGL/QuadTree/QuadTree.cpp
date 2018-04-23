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

using namespace std;

QuadTree::QuadTree() {
	readFile(this->mesh, "./../Tools/Heightmap/heightmap.jpg");

	this->cols = sqrt((int) (this->mesh.size()/3.0f));
	this->rows = this->cols;

    this->depth = floor(log2(this->cols))-1;
    this->section = 1.0f/this->depth;

    Box rootBox(0, 0, this->rows-1, this->cols-1);
    this->fillTree(this->tree, this->depth, rootBox, 0);
}

void QuadTree::getPlaneVertices(vector<float> & container, int coord) {
    container.push_back(this->mesh[coord*3]);
    container.push_back(this->mesh[coord*3+1]);
    container.push_back(this->mesh[coord*3+2]);
};

void QuadTree::fillTree(Node & currentNode, int depth, Box & currentBox, int index) {
    if(depth == 0 || !currentBox.checkPartition())
        return;

    Node node;
    node.children.resize(4);

    currentNode.children.push_back(node);
    currentNode.currentBox = currentBox;

    vector<int> vertixTriangleCords = currentBox.getVerticesCoord(this->cols);
    for(int g = 0; g < vertixTriangleCords.size(); g++)
    	this->getPlaneVertices(node.vertices, vertixTriangleCords[g]);

    vector<int> vertixLineCords = currentBox.getLinesCoord(this->cols, index);
    for(int g = 0; g < vertixLineCords.size(); g++)
    	this->getPlaneVertices(node.lines, vertixLineCords[g]);

    vector<Box> quadBox = currentBox.getPartitions();
    for(int g = 0; g < 4; g++)
    	this->fillTree(node.children[g], depth-1, quadBox[g], g);
};

void QuadTree::readDepth(vector<float> & data, int depth) {
    for(int g = 0; g < 4; g++)
        this->readLevel(data, depth-1, this->tree.children[g]);
};

void QuadTree::readLevel(vector<float> & data, int depth, Node & currentNode) {
    if(depth == 0 || !currentNode.children.empty()) {
    	for(int g = 0; g < currentNode.vertices.size(); g++)
    		data.push_back(currentNode.vertices[g]);
    } else {
        for(int g = 0; g < 4; g++)
            this->readLevel(data, depth-1, currentNode.children[g]);
    }
};

void QuadTree::checkFrustumBoundaries(Boundary & boundary, vector<float> & vertices, Matrix & projection, Matrix & viewCamera) {
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

        distance = fmax(distance, -distanceVecs(origin, projectionVector));

        if(projectionVector[0] >= -1.0 && projectionVector[0] <= 1.0 &&
            projectionVector[1] >= -1.0 && projectionVector[1] <= 1.0 &&
            projectionVector[2] >= -1.0 && projectionVector[2] <= 1.0)
            withinFrustum = true;
    }

    boundary.distance = fmax(fabs(distance), 0.01)/10;
    boundary.withinFrustum = withinFrustum;
};

void QuadTree::readProjection(vector<float> & data, Matrix & projection, Matrix & viewCamera) {
	for(int g = 0; g < 4; g++)
	    this->readComplexity(data, projection, viewCamera, this->tree.children[g], g, 1);
};

void QuadTree::readComplexity(vector<float> & data, Matrix & projection, Matrix & viewCamera, Node & currentNode, int index, int currentDepth) {
    this->checkFrustumBoundaries(this->boundary, currentNode.vertices, projection, viewCamera);

    if(this->boundary.withinFrustum) {
        int depth = this->depth-ceil((int) this->boundary.distance/this->section);

        if(depth <= currentDepth) {
            //Even complexity, or inherited from the parent previous complexity
            for(int g = 0; g < currentNode.vertices.size(); g++)
            	data.push_back(currentNode.vertices[g]);
        } else if(depth > currentDepth) {
            //Needs more complexity
            for(int g = 0; g < 4; g++)
            	this->readComplexity(data, projection, viewCamera, currentNode.children[g], g, currentDepth+1);
        }
    }
};