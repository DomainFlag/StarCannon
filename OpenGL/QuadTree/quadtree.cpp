#include "iostream"
#include <vector>
#include "QuadTree.h"

using namespace std;

int main() {
	QuadTree * quadtree = new QuadTree;

	quadtree->tree->children[0]->children[0]->children[0]->children[0]->box.print();

	delete quadtree;

	return 0;
}