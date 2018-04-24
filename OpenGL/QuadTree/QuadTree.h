#ifndef QUADTREE
#define QUADTREE

#include <iostream>
#include <vector>
#include "./../Box/Box.h"
#include "./../Node/Node.h"
#include "./../Tools/Matrix/Matrix.h"

using namespace std;

struct Boundary {
	float distance;
	bool withinFrustum;
};

class QuadTree {
public:
	Node * tree;

	vector<float> origin{0, 0, 0, 0};

	Boundary boundary;

	vector<vector<float>> mesh;
	int cols, rows;

	int depth;
	float section;

	QuadTree();

	~QuadTree();

	void fillTree(Node * currentNode, int depth, int index);

	void checkFrustumBoundaries(vector<float> & vertices, Matrix & projection, Matrix & viewCamera);

	void readProjection(vector<float> & data, Matrix & projection, Matrix & viewCamera);

	void readComplexity(vector<float> & data, Matrix & projection, Matrix & viewCamera, Node * currentNode, int index, int currentDepth);
};

#endif