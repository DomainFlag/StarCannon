#ifndef QUADTREE
#define QUADTREE

#include <iostream>
#include <vector>
#include "./../Box/Box.h"
#include "./../Tools/Matrix/Matrix.h"

using namespace std;

struct Node {
	vector<Node> children;

	vector<float> vertices;
	vector<float> lines;
	Box currentBox;
};

struct Boundary {
	float distance;
	bool withinFrustum;
};

class QuadTree {
public:
	Node tree;

	vector<float> origin{0, 0, 0, 0};

	Boundary boundary;

	vector<int> mesh;
	int cols, rows;

	int depth;
	float section;

	QuadTree();

	void fillTree(Node & currentNode, int depth, Box & currentBox, int index);

	void getPlaneVertices(vector<float> & container, int coord);

	void readDepth(vector<float> & data, int depth);

	void readLevel(vector<float> & data, int depth, Node & currentNode);

	void checkFrustumBoundaries(Boundary & boundary, vector<float> & vertices, Matrix & projection, Matrix & viewCamera);

	void readProjection(vector<float> & data, Matrix & projection, Matrix & viewCamera);

	void readComplexity(vector<float> & data, Matrix & projection, Matrix & viewCamera, Node & currentNode, int index, int currentDepth);
};

#endif