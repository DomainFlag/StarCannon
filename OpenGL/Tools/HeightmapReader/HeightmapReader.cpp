#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <SOIL/SOIL.h>

#include "HeightmapReader.h"

using namespace std;

void readFile(vector<int> & data, string pathname) {
	int width, height, channel;
	
	unsigned char * pixels = SOIL_load_image(pathname.c_str(), &width, &height, &channel, SOIL_LOAD_RGB);

	if(pixels == NULL) {
		cout << "Error while loading texture" << endl;
		exit(1);
	}

	int max_size = min(width, height);

	data.resize(max_size*max_size);

	for(int g = 0; g < max_size; g++) {
		for(int h = 0; h < max_size; h++) {
			data.push_back((int) pixels[(g + h * max_size) * 3 + 0]);
			data.push_back((int) pixels[(g + h * max_size) * 3 + 1]);
			data.push_back((int) pixels[(g + h * max_size) * 3 + 2]);
		}
	}

	SOIL_free_image_data(pixels);
};