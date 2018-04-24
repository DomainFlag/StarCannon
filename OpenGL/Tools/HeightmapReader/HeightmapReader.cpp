#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <SOIL/SOIL.h>

#include "HeightmapReader.h"

using namespace std;

void readFile(vector<vector<float>> & data, string pathname) {
	int width, height, channel;
	
	unsigned char * pixels = SOIL_load_image(pathname.c_str(), &width, &height, &channel, SOIL_LOAD_RGB);

	if(pixels == NULL) {
		cout << "Error while loading texture" << endl;
		exit(1);
	}

	int max_size = min(width, height);

	int tSize = max_size*max_size;
	data.resize(tSize);

	int counter = 0;
	float section = 12.0f/max_size;

	for(int g = 0; g < max_size; g++) {
		for(int h = 0; h < max_size; h++) {
			data[counter] = vector<float> {
				section*h-6.0f,
				((float) (int) pixels[(g + h * width) * 3])*4.0f/255.0f-2.0f,
				section*g-6.0f
			};
			counter++;
		}
	}

	SOIL_free_image_data(pixels);
};