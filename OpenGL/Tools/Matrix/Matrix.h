#ifndef MATRIX
#define MATRIX

#include <vector>
using namespace std;

class Matrix {
public:
	float * matrix = new float[16]{0};

	Matrix();

	~Matrix();

	void printMatrix();

	void translation(float x, float y, float z);

	void rotationX(float rotation);

	void rotationY(float rotation);

	void rotationZ(float rotation);

	void scalling(float x, float y, float z);

	void projection(float x, float y, float z);

	void multiplyMatrices(Matrix * mat1, Matrix * mat2);

	void multiplyVector(vector<float> vec, vector<float> & data);

	void inverseMatrix(Matrix * matt);
};

#endif