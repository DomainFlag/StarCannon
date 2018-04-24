#ifndef BOX
#define BOX

#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

class Box {
public:
	int x0, x1;
	int y0, y1;

	Box();

	Box(int x0, int y0, int x1, int y1);

	void print();

	bool checkPartition();

	vector<Box> getPartitions();

	bool checkBoundary(int tCols);

	vector<int> getVerticesCoord(int tCols);

	vector<int> getLinesCoord(int tCols, int index);
	
	vector<Box> getAdjacentBoxes(int tCols, int index);
};

#endif