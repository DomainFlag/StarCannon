#ifndef MATRIX
#define MATRIX

#include <vector>
using namespace std;

vector<float> quaternion();

vector<float> fromEuler(float x, float y, float z);

vector<float> transformQuat(vector<float> vec, vector<float> quaternion);

void fromQuat(vector<float> quaternion);

vector<float> cross(vector<float> a, vector<float> b);

vector<float> addValues(vector<float> a, vector<float> b);

vector<float> substractValues(vector<float> a, vector<float> b);

float dot(vector<float> a, vector<float> b);

vector<float> normalize(vector<float> v);

float angle(vector<float> a, vector<float> b);

float distanceVecs(vector<float> a, vector<float> b);

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

	void scaling(float x, float y, float z);

	void projection(float x, float y, float z);

	void perspective(float fieldOfView, float aspect, float near, float far);

	void lookAt(vector<float> cameraPosition, vector<float> target, vector<float> up);

	Matrix operator * (const Matrix mat);

	vector<float> multiplyVector(vector<float> vec);

	Matrix inverseMatrix(Matrix matt);

	void fromQuat(vector<float> quaternion);

	Matrix transposeMatrix(Matrix matrix);
};

#endif