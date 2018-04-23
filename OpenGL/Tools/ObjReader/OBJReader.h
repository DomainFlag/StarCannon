#ifndef OBJREADER
#define OBJREADER

#include <vector>
#include <string>
#include <map>
using namespace std;

struct Obj {
	vector<string> textures;
	string mtl;
	string obj;
};

struct Material {
	Material(string texture, int unit) : texture(texture), unit(unit) {}
	string texture;
	int unit;
};

struct Data {
	vector<string> textures;
	vector<float> geometricVertices;
	vector<float> textureVertices;
	vector<float> normalVertices;
};

void readDir(Data & data, string pathname);
map<string, Material> readMtl(string pathname, string mtlPath);
void readObj(Data & data, string pathname, string objPath, map<string, Material> materials);

#endif