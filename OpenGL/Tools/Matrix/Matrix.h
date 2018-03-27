#ifndef MATRIX
#define MATRIX

#include <vector>
using namespace std;

class Matrix {
public:
	vector<float> matrix = decltype(matrix)(16, 0);

	Matrix();

	~Matrix();

	void printMatrix();

	void translation(float x, float y, float z);

	void rotationX(float rotation);

	void rotationY(float rotation);

	void rotationZ(float rotation);

	void scalling(float x, float y, float z);

	void projection(float x, float y, float z);

	void perspective(float fieldOfView, float aspect, float near, float far);

	Matrix operator * (const Matrix mat);

	vector<float> multiplyVector(vector<float> vec);

	Matrix inverseMatrix(Matrix matt);

	vector<float> quaternion();

	vector<float> fromEuler(float x, float y, float z);

	void fromQuat(vector<float> quaternion);

	vector<float> transformQuat(vector<float> vec, vector<float> quaternion);
};

#endif