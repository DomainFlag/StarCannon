#include "Matrix.h"
#include <iostream>
#include <math.h>
#include <vector>
using namespace std;

Matrix::Matrix() {
	for(int g = 0; g < 4; g++)
		this->matrix[g*4+g] = 1;
};

Matrix::~Matrix() {
	//
}

void Matrix::printMatrix() {
	for(int g = 0; g < 4; g++) {
		for(int h = 0; h < 4; h++) {
			cout << this->matrix[g*4+h] << " ";
		}
		cout << endl;
	}
}

void Matrix::translation(float x, float y, float z ) {
	this->matrix[12] = x;
	this->matrix[13] = y;
	this->matrix[14] = z;
}

void Matrix::rotationX(float rotation) {
	float c = cos(rotation);
	float s = sin(rotation);
	this->matrix[5] = c;
	this->matrix[6] = s;
	this->matrix[9] = -s;
	this->matrix[10] = c;
}


void Matrix::rotationY(float rotation) {
	float c = cos(rotation);
	float s = sin(rotation);
	this->matrix[0] = c;
	this->matrix[2] = -s;
	this->matrix[8] = s;
	this->matrix[10] = c;
}


void Matrix::rotationZ(float rotation) {
	float c = cos(rotation);
	float s = sin(rotation);
	this->matrix[0] = c;
	this->matrix[1] = s;
	this->matrix[4] = -s;
	this->matrix[5] = c;
}

void Matrix::scalling(float x, float y, float z) {
	this->matrix[0] = x;
	this->matrix[5] = y;
	this->matrix[10] = z;
	this->matrix[15] = 1;
}

void Matrix::projection(float x, float y, float z) {
	this->matrix[0] = 2.0f/x;
	this->matrix[5] = -2.0f/y;
	this->matrix[10] = 1.0f/z;
	this->matrix[12] = -1.0f;
	this->matrix[13] = 1.0f;
	this->matrix[15] = 1.0f;
}

void Matrix::perspective(float fieldOfView, float aspect, float near, float far) {
	float f = tan(M_PI*0.5 - 0.5*fieldOfView);
	float rangeInv = 1.0/(near-far);

	this->matrix[0] = f/aspect;
	this->matrix[5] = f;
	this->matrix[10] = (near+far)*rangeInv;
	this->matrix[11] = -1;
	this->matrix[14] = near*far*rangeInv*2;
}

Matrix Matrix::operator * (const Matrix mat) {
	Matrix result;
	for(int g = 0; g < 4; g++) {
		for(int h = 0; h < 4; h++) {
			result.matrix[g*4+h] = 0;
			for(int i = 0; i < 4; i++) {
				result.matrix[g*4+h] += this->matrix[g*4+i]*mat.matrix[i*4+h];
			}
		}
	}
	return result;
}

vector<float> Matrix::multiplyVector(vector<float> vec) {
	vector<float> data(4, 0);
	for(int h = 0; h < 4; h++) {
		for(int i = 0; i < 4; i++) {
			data[h] += this->matrix[h*4+i]*vec[i];
		}
	}
	return data;
}

vector<float> Matrix::quaternion() {
    vector<float> out(4, 0);

    out[0] = 0;
    out[1] = 0;
    out[2] = 0;
    out[3] = 1;

    return out;
}

vector<float> Matrix::fromEuler(float x, float y, float z) {
	vector<float> data(4, 0);

    float halfToRad = 0.5*M_PI/180.0;
    x *= halfToRad;
    y *= halfToRad;
    z *= halfToRad;
    float sx = sin(x);
    float cx = cos(x);
    float sy = sin(y);
    float cy = cos(y);
    float sz = sin(z);
    float cz = cos(z);
    data[0] = sx * cy * cz - cx * sy * sz;
    data[1] = cx * sy * cz + sx * cy * sz;
    data[2] = cx * cy * sz - sx * sy * cz;
    data[3] = cx * cy * cz + sx * sy * sz;

    return data;
}

void Matrix::fromQuat(vector<float> quaternion) {
	float x = quaternion[0], y = quaternion[1], z = quaternion[2], w = quaternion[3];
    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;
    float xx = x * x2;
    float yx = y * x2;
    float yy = y * y2;
    float zx = z * x2;
    float zy = z * y2;
    float zz = z * z2;
    float wx = w * x2;
    float wy = w * y2;
    float wz = w * z2;
    this->matrix[0] = 1 - yy - zz;
    this->matrix[1] = yx + wz;
    this->matrix[2] = zx - wy;
    this->matrix[3] = 0;
    this->matrix[4] = yx - wz;
    this->matrix[5] = 1 - xx - zz;
    this->matrix[6] = zy + wx;
    this->matrix[7] = 0;
    this->matrix[8] = zx + wy;
    this->matrix[9] = zy - wx;
    this->matrix[10] = 1 - xx - yy;
    this->matrix[11] = 0;
    this->matrix[12] = 0;
    this->matrix[13] = 0;
    this->matrix[14] = 0;
    this->matrix[15] = 1;
}

vector<float> Matrix::transformQuat(vector<float> vec, vector<float> quaternion) {
	vector<float> data(3, 0);

    float x = vec[0], y = vec[1], z = vec[2];
    float qx = quaternion[0], qy = quaternion[1], qz = quaternion[2], qw = quaternion[3];

    float ix = qw * x + qy * z - qz * y;
    float iy = qw * y + qz * x - qx * z;
    float iz = qw * z + qx * y - qy * x;
    float iw = -qx * x - qy * y - qz * z;

    data[0] = ix * qw + iw * -qx + iy * -qz - iz * -qy;
    data[1] = iy * qw + iw * -qy + iz * -qx - ix * -qz;
    data[2] = iz * qw + iw * -qz + ix * -qy - iy * -qx;
    
    return data;
}

Matrix Matrix::inverseMatrix(Matrix matt) {
	Matrix result;
	const vector<float> & mat = matt.matrix;

	float m00 = mat[0 * 4 + 0];
	float m01 = mat[0 * 4 + 1];
	float m02 = mat[0 * 4 + 2];
	float m03 = mat[0 * 4 + 3];
	float m10 = mat[1 * 4 + 0];
	float m11 = mat[1 * 4 + 1];
	float m12 = mat[1 * 4 + 2];
	float m13 = mat[1 * 4 + 3];
	float m20 = mat[2 * 4 + 0];
	float m21 = mat[2 * 4 + 1];
	float m22 = mat[2 * 4 + 2];
	float m23 = mat[2 * 4 + 3];
	float m30 = mat[3 * 4 + 0];
	float m31 = mat[3 * 4 + 1];
	float m32 = mat[3 * 4 + 2];
	float m33 = mat[3 * 4 + 3];
	float tmp_0  = m22 * m33;
	float tmp_1  = m32 * m23;
	float tmp_2  = m12 * m33;
	float tmp_3  = m32 * m13;
	float tmp_4  = m12 * m23;
	float tmp_5  = m22 * m13;
	m11 = mat[1 * 4 + 1];
	m12 = mat[1 * 4 + 2];
	m13 = mat[1 * 4 + 3];
	m20 = mat[2 * 4 + 0];
	m21 = mat[2 * 4 + 1];
	m22 = mat[2 * 4 + 2];
	m23 = mat[2 * 4 + 3];
	m30 = mat[3 * 4 + 0];
	m31 = mat[3 * 4 + 1];
	m32 = mat[3 * 4 + 2];
	m33 = mat[3 * 4 + 3];
	tmp_0  = m22 * m33;
	tmp_1  = m32 * m23;
	tmp_2  = m12 * m33;
	tmp_3  = m32 * m13;
	tmp_4  = m12 * m23;
	tmp_5  = m22 * m13;
	float tmp_6  = m02 * m33;
	float tmp_7  = m32 * m03;
	float tmp_8  = m02 * m23;
	float tmp_9  = m22 * m03;
	float tmp_10 = m02 * m13;
	float tmp_11 = m12 * m03;
	float tmp_12 = m20 * m31;
	float tmp_13 = m30 * m21;
	float tmp_14 = m10 * m31;
	float tmp_15 = m30 * m11;
	float tmp_16 = m10 * m21;
	float tmp_17 = m20 * m11;
	float tmp_18 = m00 * m31;
	float tmp_19 = m30 * m01;
	float tmp_20 = m00 * m21;
	float tmp_21 = m20 * m01;
	float tmp_22 = m00 * m11;
	float tmp_23 = m10 * m01;

	float t0 = (tmp_0 * m11 + tmp_3 * m21 + tmp_4 * m31) -
	    (tmp_1 * m11 + tmp_2 * m21 + tmp_5 * m31);
	float t1 = (tmp_1 * m01 + tmp_6 * m21 + tmp_9 * m31) -
	    (tmp_0 * m01 + tmp_7 * m21 + tmp_8 * m31);
	float t2 = (tmp_2 * m01 + tmp_7 * m11 + tmp_10 * m31) -
	    (tmp_3 * m01 + tmp_6 * m11 + tmp_11 * m31);
	float t3 = (tmp_5 * m01 + tmp_8 * m11 + tmp_11 * m21) -
	    (tmp_4 * m01 + tmp_9 * m11 + tmp_10 * m21);

	float d = 1.0 / (m00 * t0 + m10 * t1 + m20 * t2 + m30 * t3);

	result.matrix[0] = d*t0;
	result.matrix[1] = d*t1;
	result.matrix[2] = d*t2;
	result.matrix[3] = d*t3;

	result.matrix[4] = d * ((tmp_1 * m10 + tmp_2 * m20 + tmp_5 * m30) -
	        (tmp_0 * m10 + tmp_3 * m20 + tmp_4 * m30));
	result.matrix[5] = d * ((tmp_0 * m00 + tmp_7 * m20 + tmp_8 * m30) -
	        (tmp_1 * m00 + tmp_6 * m20 + tmp_9 * m30));
	result.matrix[6] = d * ((tmp_3 * m00 + tmp_6 * m10 + tmp_11 * m30) -
	        (tmp_2 * m00 + tmp_7 * m10 + tmp_10 * m30));
	result.matrix[7] = d * ((tmp_4 * m00 + tmp_9 * m10 + tmp_10 * m20) -
	        (tmp_5 * m00 + tmp_8 * m10 + tmp_11 * m20));

	result.matrix[8] = d * ((tmp_12 * m13 + tmp_15 * m23 + tmp_16 * m33) -
	        (tmp_13 * m13 + tmp_14 * m23 + tmp_17 * m33));
	result.matrix[9] = d * ((tmp_13 * m03 + tmp_18 * m23 + tmp_21 * m33) -
	        (tmp_12 * m03 + tmp_19 * m23 + tmp_20 * m33));
	result.matrix[10] = d * ((tmp_14 * m03 + tmp_19 * m13 + tmp_22 * m33) -
	        (tmp_15 * m03 + tmp_18 * m13 + tmp_23 * m33));
	result.matrix[11] = d * ((tmp_17 * m03 + tmp_20 * m13 + tmp_23 * m23) -
	        (tmp_16 * m03 + tmp_21 * m13 + tmp_22 * m23));


	result.matrix[12] = d * ((tmp_14 * m22 + tmp_17 * m32 + tmp_13 * m12) -
	        (tmp_16 * m32 + tmp_12 * m12 + tmp_15 * m22));
	result.matrix[13] = d * ((tmp_20 * m32 + tmp_12 * m02 + tmp_19 * m22) -
	        (tmp_18 * m22 + tmp_21 * m32 + tmp_13 * m02));
	result.matrix[14] = d * ((tmp_18 * m12 + tmp_23 * m32 + tmp_15 * m02) -
	        (tmp_22 * m32 + tmp_14 * m02 + tmp_19 * m12));
	result.matrix[15] = d * ((tmp_22 * m22 + tmp_16 * m02 + tmp_21 * m12) -
	        (tmp_20 * m12 + tmp_23 * m22 + tmp_17 * m02));

	return result;
}