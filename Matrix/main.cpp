#include <iostream>
#include <map>
#include <string>
using namespace std;

class Matrix {

	//TODO (1) Print matrix method:
	//Prints a matrix to the console(1D array of 16 float values), good for latter debugging.

	//TODO (2) Transformation matrices:

	//Define 9 matrix methods(Membres classe):
	//1. Translation matrix
	//2. Rotation matrix(x, y, z)
	//3. Scale matrix
	//4. Perspective matrix
	//5. Projection matrix
	//Every matrix takes as an input the necessary number of arguments of float type
	//and it should return an array of 16 floats(2D array disguised in 1D array. See the *1.

	//TODO (3) Multiplication matrix method:
	// It takes a (int) nb that represents the nb of arguments and an 1D array of matrices of 16 floats
	// And it multiples starting with first to last, order is important.  

	//TODO (4) Inverse matrix method:
	// Hint: https://en.wikipedia.org/wiki/Gaussian_elimination
};

// 1* | (below)
//    v

// 1.
// Translation matrix:
// 1,  0,  0,  0,
// 0,  1,  0,  0,
// 0,  0,  1,  0,
// tx, ty, tz, 1

// c -> cos of angle in radians
// s -> sin of angle in radians

// 2.
// RotationX matrix:
// 1, 0, 0, 0,
// 0, c, s, 0,
// 0, -s, c, 0,
// 0, 0, 0, 1,

// 3.
// RotationY matrix:
// c, 0, -s, 0,
// 0, 1, 0, 0,
// s, 0, c, 0,
// 0, 0, 0, 1,

// 4.
// RotationZ matrix:
// c, s, 0, 0,
// -s, c, 0, 0,
// 0, 0, 1, 0,
// 0, 0, 0, 1,

// 5.
// Scalling matrix:
// sx, 0,  0,  0,
// 0, sy,  0,  0,
// 0,  0, sz,  0,
// 0,  0,  0,  1,

// 6.
// Perspective matrix, parameters(fieldOfViewInRadians, aspect, near, far):
// float f = Tan(PI * 0.5 - 0.5 * fieldOfViewInRadians); //Tan->tangent
// float rangeInv = 1.0 / (near - far);

// f / aspect, 0, 0, 0,
// 0, f, 0, 0,
// 0, 0, (near + far) * rangeInv, -1,
// 0, 0, near * far * rangeInv * 2, 0

// 7.
// Projection matrix:
// 2 / width, 0, 0, 0,
// 0, -2 / height, 0, 0,
// 0, 0, 2 / depth, 0,
// -1, 1, 0, 1


//TODO (5) Daca ai dispozitie, iata un exemplu cum se foloseste map class,
//Ar fi nice daca am avea un map unde valorile key sunt(translationX, ...) si ele returneaza
//o functie pointer care apelindo ne da matricea care avem nevoie.

// void apple(int argc, string * argv) {
// 	cout << "Yeah! delicious!";
// }

// void candy(int nb, string * data) {
// 	if(nb == 1)
// 		cout << "Yeah! godly delicious! " << data[0] << endl;
// }

// int main() {
// 	map<string, void (*)(int argc, string * argv)> data;
// 	data.insert(pair<string, void (*)(int argc, string * argv)>("apple", &apple));
// 	data["candy"] = &candy;

// 	string argv[1] = {"Oh yea"};
// 	data["candy"](1, argv);

// 	return 0;
// }