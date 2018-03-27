#include "Matrix.h"
#include <iostream>
using namespace std;

int main() {
	Matrix matrix1;
	matrix1.rotationX(0.2);
	Matrix matrix2;
	matrix2.rotationY(0.5);
	Matrix matrix3;
	matrix3.rotationZ(0.2);

	Matrix matrix = matrix1*matrix2;
	
    return 0;
}