#include <iostream>
#include <map>
#include <string>
#include <math.h>
#include <assert.h>
using namespace std;

class Matrix {
public:
	float * matrix = new float[16]{0};

	Matrix() {
		for(int g = 0; g < 4; g++)
			this->matrix[g*4+g] = 1;
	};

	~Matrix() {
		delete [] this->matrix;
		this->matrix = NULL;
	}

	void printMatrix() {
		for(int g = 0; g < 4; g++) {
			for(int h = 0; h < 4; h++) {
				cout << this->matrix[g*4+h] << " ";
			}
			cout << endl;
		}
	}

	void translation(float x, float y, float z ) {
		this->matrix[12] = x;
		this->matrix[13] = y;
		this->matrix[14] = z;
	}

	void rotationX(float rotation) {
		float c = cos(rotation);
		float s = sin(rotation);
		this->matrix[5] = c;
		this->matrix[6] = s;
		this->matrix[9] = -s;
		this->matrix[10] = c;
	}


	void rotationY(float rotation) {
		float c = cos(rotation);
		float s = sin(rotation);
		this->matrix[0] = c;
		this->matrix[2] = -s;
		this->matrix[8] = s;
		this->matrix[10] = c;
	}


	void rotationZ(float rotation) {
		float c = cos(rotation);
		float s = sin(rotation);
		this->matrix[0] = c;
		this->matrix[1] = s;
		this->matrix[4] = -s;
		this->matrix[5] = c;
	}

	void scalling(float x, float y, float z) {
		this->matrix[0] = x;
		this->matrix[5] = y;
		this->matrix[10] = z;
		this->matrix[15] = 1;
	}

	void projection(float x, float y, float z) {
		this->matrix[0] = 2.0f/x;
		this->matrix[5] = -2.0f/y;
		this->matrix[10] = 1.0f/z;
		this->matrix[12] = -1.0f;
		this->matrix[13] = 1.0f;
		this->matrix[15] = 1.0f;
	}

	void multiplyMatrices(Matrix * mat1, Matrix * mat2) {
		for(int g = 0; g < 4; g++) {
			for(int h = 0; h < 4; h++) {
				this->matrix[g*4+h] = 0;
				for(int i = 0; i < 4; i++) {
					this->matrix[g*4+h] += mat1->matrix[g*4+i]*mat2->matrix[i*4+h];
				}
			}
		}
	}
};