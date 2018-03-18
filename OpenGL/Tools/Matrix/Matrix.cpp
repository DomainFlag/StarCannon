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
	delete [] this->matrix;
	this->matrix = NULL;
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

void Matrix::multiplyMatrices(Matrix * mat1, Matrix * mat2) {
	for(int g = 0; g < 4; g++) {
		for(int h = 0; h < 4; h++) {
			this->matrix[g*4+h] = 0;
			for(int i = 0; i < 4; i++) {
				this->matrix[g*4+h] += mat1->matrix[g*4+i]*mat2->matrix[i*4+h];
			}
		}
	}
}

void Matrix::multiplyVector(vector<float> vec, vector<float> & data) {
	for(int h = 0; h < 4; h++) {
		for(int i = 0; i < 4; i++) {
			data[h] += this->matrix[h*4+i]*vec[i];
		}
	}
}

void Matrix::inverseMatrix(Matrix * matt) {
	float * mat = matt->matrix;

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

	this->matrix[0] = d*t0;
	this->matrix[1] = d*t1;
	this->matrix[2] = d*t2;
	this->matrix[3] = d*t3;

	this->matrix[4] = d * ((tmp_1 * m10 + tmp_2 * m20 + tmp_5 * m30) -
	        (tmp_0 * m10 + tmp_3 * m20 + tmp_4 * m30));
	this->matrix[5] = d * ((tmp_0 * m00 + tmp_7 * m20 + tmp_8 * m30) -
	        (tmp_1 * m00 + tmp_6 * m20 + tmp_9 * m30));
	this->matrix[6] = d * ((tmp_3 * m00 + tmp_6 * m10 + tmp_11 * m30) -
	        (tmp_2 * m00 + tmp_7 * m10 + tmp_10 * m30));
	this->matrix[7] = d * ((tmp_4 * m00 + tmp_9 * m10 + tmp_10 * m20) -
	        (tmp_5 * m00 + tmp_8 * m10 + tmp_11 * m20));

	this->matrix[8] = d * ((tmp_12 * m13 + tmp_15 * m23 + tmp_16 * m33) -
	        (tmp_13 * m13 + tmp_14 * m23 + tmp_17 * m33));
	this->matrix[9] = d * ((tmp_13 * m03 + tmp_18 * m23 + tmp_21 * m33) -
	        (tmp_12 * m03 + tmp_19 * m23 + tmp_20 * m33));
	this->matrix[10] = d * ((tmp_14 * m03 + tmp_19 * m13 + tmp_22 * m33) -
	        (tmp_15 * m03 + tmp_18 * m13 + tmp_23 * m33));
	this->matrix[11] = d * ((tmp_17 * m03 + tmp_20 * m13 + tmp_23 * m23) -
	        (tmp_16 * m03 + tmp_21 * m13 + tmp_22 * m23));


	this->matrix[12] = d * ((tmp_14 * m22 + tmp_17 * m32 + tmp_13 * m12) -
	        (tmp_16 * m32 + tmp_12 * m12 + tmp_15 * m22));
	this->matrix[13] = d * ((tmp_20 * m32 + tmp_12 * m02 + tmp_19 * m22) -
	        (tmp_18 * m22 + tmp_21 * m32 + tmp_13 * m02));
	this->matrix[14] = d * ((tmp_18 * m12 + tmp_23 * m32 + tmp_15 * m02) -
	        (tmp_22 * m32 + tmp_14 * m02 + tmp_19 * m12));
	this->matrix[15] = d * ((tmp_22 * m22 + tmp_16 * m02 + tmp_21 * m12) -
	        (tmp_20 * m12 + tmp_23 * m22 + tmp_17 * m02));
}