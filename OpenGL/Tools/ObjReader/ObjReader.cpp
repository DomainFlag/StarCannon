#include <dirent.h>
#include <iostream>
#include <vector>
#include <regex>
#include <boost/regex.hpp>
#include <map>
#include <fstream>
#include "ObjReader.h"
using namespace std;

void readObj(Data & data, string pathname, string objPath, map<string, Material> materials) {
	vector<float> geometricVertixIndices;
	vector<float> textureVertixIndices;
	vector<float> normalVertixIndices;

	ifstream objfile((pathname+objPath).c_str());

	if(objfile.is_open()) {
		string line;
		smatch matches;
		string material;
		int texUnit = 0;
		bool indexesFlag = true;

		match_results<string::const_iterator> testMatches;

		regex geoVertixIndices("v\\s(-?\\d+\\.\\d+)\\s(-?\\d+\\.\\d+)\\s(-?\\d+\\.\\d+)");
		regex texVertixIndices("vt\\s(-?\\d+\\.\\d+)\\s(-?\\d+\\.\\d+)");
		regex norVertixIndices("vn\\s(-?\\d+\\.\\d+)\\s(-?\\d+\\.\\d+)\\s(-?\\d+\\.\\d+)");

		regex vertexRegex("f\\s(\\d+)/(\\d+)/(\\d+)\\s(\\d+)/(\\d+)/(\\d+)\\s(\\d+)/(\\d+)/(\\d+)");
		regex materialRegex("usemtl\\s(\\w+:)?(\\w+)");

		while(indexesFlag && getline(objfile, line)) {
			if(regex_search(line, matches, geoVertixIndices)) {
				geometricVertixIndices.push_back(stof(matches[1]));
				geometricVertixIndices.push_back(stof(matches[2]));
				geometricVertixIndices.push_back(stof(matches[3]));
			} else if(regex_search(line, matches, texVertixIndices)) {
				indexesFlag = false;
			}
		}

		indexesFlag = true;

		do {
			if(regex_search(line, matches, texVertixIndices)) {
				textureVertixIndices.push_back(stof(matches[1]));
				textureVertixIndices.push_back(stof(matches[2]));
			} else if(regex_search(line, matches, norVertixIndices)) {
				indexesFlag = false;
			}
		} while(indexesFlag && getline(objfile, line));

		indexesFlag = true;

		do {
			if(regex_search(line, matches, norVertixIndices)) {
				normalVertixIndices.push_back(stof(matches[1]));
				normalVertixIndices.push_back(stof(matches[2]));
				normalVertixIndices.push_back(stof(matches[3]));
			} else if(regex_search(line, matches, materialRegex)) {
				indexesFlag = false;
			}
		} while(indexesFlag && getline(objfile, line));


		if(!indexesFlag)
			do {
				if(regex_search(line, matches, vertexRegex)) {
					for(int g = 1; g < 9; g += 3) {
						for(int h = 0; h < 3; h++) {
							data.geometricVertices.push_back(geometricVertixIndices[stoi(matches[g])*3+h]);
							data.normalVertices.push_back(normalVertixIndices[stoi(matches[g+2])*3+h]);
						}
						for(int h = 0; h < 2; h++)
							data.textureVertices.push_back(textureVertixIndices[stoi(matches[g+1])*2+h]);
						data.textureVertices.push_back(texUnit);
					}
				} else if(regex_search(line, matches, materialRegex)) {
					material = matches[2];
					texUnit = materials.find(material)->second.unit;
				}
			} while(getline(objfile, line));
	}


	for(map<string, Material>::iterator it = materials.begin(); it != materials.end(); it++)
		if(it->second.unit != -1) {
			data.textures.push_back(Texture(it->second.texture, it->second.unit));
		}
}

map<string, Material> readMtl(string pathname, string mtlPath) {
    map<string, Material> materials;

    int counter = 0;
    ifstream mtlfile((pathname+mtlPath).c_str());

    if(mtlfile.is_open()) {
    	string line;
    	cmatch matches;

    	string material;
    	while(getline(mtlfile, line)) {
    		if(regex_search(line.c_str(), matches, regex("newmtl\\s(\\w+:)?(\\w+)"))) {
    			if(matches.size() >= 2) {
    				if(!material.empty()) {
    					materials.insert(pair<string, Material>(
    						material,
    						Material(
    							"",
    							-1)
    						));
    					material.clear();
    				}
    				material = matches[2];
    			}
    		} else if(regex_search(line.c_str(), matches, regex("map_Kd\\s(.*\\.png)"))) {
    			if(matches.size() >= 1) {
    				if(!material.empty()) {
    					materials.insert(pair<string, Material>(
    						material,
    						Material(
    							matches[1],
    							counter++)
    						));
    					material.clear();
    				}
    			}
    		}
    	}
    } else {
    	cout << "Couldn't open the MTL file" << endl;
    }

    return materials;
}

void readDir(Data & data, string pathname) {
	DIR           * dirp;
	struct dirent * directory;

	dirp = opendir(pathname.c_str());
	Obj obj;

	if(dirp) {
	    while((directory = readdir(dirp)) != NULL) {
	    	if(regex_match(directory->d_name, regex("^[^\\.]+.*\\.png") ))
	    		obj.textures.push_back(directory->d_name);
	    	else if(regex_match(directory->d_name, regex("^[^\\.]+.*\\.obj"))) {
	    		obj.obj = directory->d_name;
	    	} else if(regex_match(directory->d_name, regex("^[^\\.]+.*\\.mtl"))) {
	    		obj.mtl = directory->d_name;
	    	}
	    }

	    closedir(dirp);
	}

	map<string, Material> materials = readMtl(pathname, obj.mtl);
	readObj(data, pathname, obj.obj, materials);
}