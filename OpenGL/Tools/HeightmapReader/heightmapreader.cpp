#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

#include "HeightmapReader.h"

using namespace std;

int main() {
	vector<vector<float>> data;
	readFile(data, "./../Heightmap/heightmap.jpg");

	return 0;
}