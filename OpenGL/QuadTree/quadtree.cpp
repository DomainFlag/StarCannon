#include "iostream"
#include <vector>
#include "QuadTree.h"

using namespace std;

int main() {
	QuadTree quadtree;
	cout << quadtree.tree.children[0].vertices[0] << endl;
	cout << quadtree.tree.children[0].vertices[1] << endl;
	cout << quadtree.tree.children[0].vertices[2] << endl;
	cout << quadtree.tree.children[0].vertices[3] << endl;
	return 0;
}