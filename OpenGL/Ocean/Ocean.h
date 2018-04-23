#ifndef OCEANLAYER
#define OCEANLAYER

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <SOIL/SOIL.h>
#include "./../Tools/Matrix/Matrix.h"
using namespace std;

class Vector {
public:
    float x, y, z;

    Vector(float x, float y, float z) {
    	this->x = x;
    	this->y = y;
    	this->z = z;
    }

    vector<float> getVector() {
    	return vector<float>{this->x, this->y, this->z};
    }
};

class Ocean {
public:
	float speed = -3.0;
	vector<float> mesh;
	vector<vector<Vector>> honeycomb;
	float partitionX;
	float partitionY;

	Ocean() {}

	void setHoneycomb(const GLFWvidmode * mode) {
	    float dimX = mode->width;
	    float dimY = mode->height/2.0f;

	    this->partitionX = 18;
	    this->partitionY = 13;

	    for(float x = -this->partitionX*25.0f; x < dimX+this->partitionX*25.0f; x += this->partitionX) {
	        vector<Vector> layer;
	        for(float y = -this->partitionY*2.0f; y < dimY+this->partitionY*2.0f; y += this->partitionY) {
	        	layer.push_back(
	        		Vector {
		        		x+rand()%(int)(this->partitionX+1.0f),
		        		y+rand()%(int)(this->partitionY+1.0f),
		        		-((float)(rand()%201))
	        		});
	        }
	        this->honeycomb.push_back(layer);
	    }
	};

	void parseHoneycomb() {
		vector<vector<int>> cycle = {
			vector<int>{0, 0}, 
			vector<int>{0, 1},
			vector<int>{1, 1}, 
			vector<int>{1, 1},
			vector<int>{1, 0}, 
			vector<int>{0, 0},
		};

	    for(float g = 0; g < this->honeycomb.size()-1; g++) {
	        for(float h = 0; h < this->honeycomb[g].size()-1; h++) {

	        	for(unsigned int i = 0; i < cycle.size(); i++) {
	        		Vector fetchedVector = this->honeycomb[g+cycle[i][0]][h+cycle.at(i).at(1)];

					vector<float> data = fetchedVector.getVector();

	        		for(unsigned int j = 0; j < 3; j++)
	        			this->mesh.push_back(data[j]);
	        	}
	        }
	    }
	};

	unsigned int getNbTriangles() {
	    return this->mesh.size()/3.0f;
	};
};


class Noise {
public:
	vector<float> noise;
	float speed = 4;
	float natural = 20;
	float smoothness = 200;
	unsigned int index = 0;

	Noise() {
		this->setNoise();
	}

	void setNoise() {
		float random = rand()%201;
		this->noise.push_back(random);

		for(int h = 1; h < this->speed; h++) {
			this->noise.push_back(random);
		}

		for(int g = 1; g < this->natural; g++) {
			random = this->noise[g*this->speed-1]+(1/(this->smoothness+rand()%(int)this->smoothness));

			for(int h = 0; h < this->speed; h++) {
				this->noise.push_back(random);
			}
		}

		for(int g = this->natural*this->speed-1; g >= 0; g--) {
			this->noise.push_back(this->noise[g]);
		}
	}

	float getNoise() {
		this->index = (this->index+1)%(int)(this->natural*this->speed*2);
		return this->noise.at(this->index);
	}
};

class OceanLayer {
public:
	Matrix translation, projection, rotationX, rotationY, rotationZ;
	Matrix viewMatrix;
	float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;
	float distance = 0.0f;

	const GLFWvidmode * mode;
	float translationX = 0.0f;
	unsigned int program;

	unsigned int positionBuffer;

	Ocean ocean;
	Noise noise;

	GLint vertexPositionLocation, uniformMatrixLocation, uniformRotationLocation, uniformDepthCurrentLocation, uniformDepthMaxLocation,
	uniformTranslation, uniformRandomLocation;

	string vertexShader = R"(
		#version 130
		attribute vec4 a_position;
		varying float v_depth;
		varying float v_far;
		varying vec4 vTextureCoord;
		uniform mat4 u_matrix;
		uniform mat4 u_rotation;
		uniform float u_random;
		varying float v_random;

		vec3 mod289(vec3 x) {
		  return x - floor(x * (1.0 / 289.0)) * 289.0;
		}

		vec4 mod289(vec4 x) {
		  return x - floor(x * (1.0 / 289.0)) * 289.0;
		}

		vec4 permute(vec4 x) {
		  return mod289(((x*34.0)+1.0)*x);
		}

		vec4 taylorInvSqrt(vec4 r) {
		  return 1.79284291400159 - 0.85373472095314 * r;
		}

		vec3 fade(vec3 t) {
		  return t*t*t*(t*(t*6.0-15.0)+10.0);
		}

		// Classic Perlin noise
		float cnoise(vec3 P) {
		  vec3 Pi0 = floor(P); // Integer part for indexing
		  vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
		  Pi0 = mod289(Pi0);
		  Pi1 = mod289(Pi1);
		  vec3 Pf0 = fract(P); // Fractional part for interpolation
		  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
		  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
		  vec4 iy = vec4(Pi0.yy, Pi1.yy);
		  vec4 iz0 = Pi0.zzzz;
		  vec4 iz1 = Pi1.zzzz;

		  vec4 ixy = permute(permute(ix) + iy);
		  vec4 ixy0 = permute(ixy + iz0);
		  vec4 ixy1 = permute(ixy + iz1);

		  vec4 gx0 = ixy0 * (1.0 / 7.0);
		  vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
		  gx0 = fract(gx0);
		  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
		  vec4 sz0 = step(gz0, vec4(0.0));
		  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
		  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

		  vec4 gx1 = ixy1 * (1.0 / 7.0);
		  vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
		  gx1 = fract(gx1);
		  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
		  vec4 sz1 = step(gz1, vec4(0.0));
		  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
		  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

		  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
		  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
		  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
		  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
		  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
		  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
		  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
		  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

		  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
		  g000 *= norm0.x;
		  g010 *= norm0.y;
		  g100 *= norm0.z;
		  g110 *= norm0.w;
		  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
		  g001 *= norm1.x;
		  g011 *= norm1.y;
		  g101 *= norm1.z;
		  g111 *= norm1.w;

		  float n000 = dot(g000, Pf0);
		  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
		  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
		  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
		  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
		  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
		  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
		  float n111 = dot(g111, Pf1);

		  vec3 fade_xyz = fade(Pf0);
		  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
		  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
		  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x); 
		  return 2.2 * n_xyz;
		}

		void main() {
			float p = cnoise(vec3(a_position.xy, u_random)*10.0);
			vec4 newpos = a_position+a_position*p/20.0;

			vec4 pos = u_matrix*newpos;
			pos.y -= 1.0;

			// // Clipping triangles that their vertices are in the opposite Y direction.
			// if(pos.y < -1.0+0.04 && pos.y > -1.0)
			//     return;
			// if(pos.y < -1.0)
			//     pos.y = pos.y+1.0;
			 
			vTextureCoord = pos;

			pos.yz /= (pos.y+1.0+0.1)*10.0; //5.0
			vec4 rotatedOcean = u_rotation*pos;
			gl_Position = vec4(rotatedOcean.xyz, 1.0);
			
			v_depth = pos.z;
			v_far = pos.y;
			v_random = u_random;
		}
	)";

	string fragmentShader = R"(
		#version 130
		precision mediump float;
		varying float v_depth;
		varying float v_far;
		varying vec4 vTextureCoord;
		uniform float u_depth_current;
		uniform float u_depth_max;
		uniform sampler2D u_water;
		uniform float u_translation;
		varying float v_random;

		vec3 mod289(vec3 x) {
		  return x - floor(x * (1.0 / 289.0)) * 289.0;
		}

		vec4 mod289(vec4 x) {
		  return x - floor(x * (1.0 / 289.0)) * 289.0;
		}

		vec4 permute(vec4 x) {
		  return mod289(((x*34.0)+1.0)*x);
		}

		vec4 taylorInvSqrt(vec4 r) {
		  return 1.79284291400159 - 0.85373472095314 * r;
		}

		vec3 fade(vec3 t) {
		  return t*t*t*(t*(t*6.0-15.0)+10.0);
		}

		// Classic Perlin noise
		float cnoise(vec3 P) {
		  vec3 Pi0 = floor(P); // Integer part for indexing
		  vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
		  Pi0 = mod289(Pi0);
		  Pi1 = mod289(Pi1);
		  vec3 Pf0 = fract(P); // Fractional part for interpolation
		  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
		  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
		  vec4 iy = vec4(Pi0.yy, Pi1.yy);
		  vec4 iz0 = Pi0.zzzz;
		  vec4 iz1 = Pi1.zzzz;

		  vec4 ixy = permute(permute(ix) + iy);
		  vec4 ixy0 = permute(ixy + iz0);
		  vec4 ixy1 = permute(ixy + iz1);

		  vec4 gx0 = ixy0 * (1.0 / 7.0);
		  vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
		  gx0 = fract(gx0);
		  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
		  vec4 sz0 = step(gz0, vec4(0.0));
		  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
		  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

		  vec4 gx1 = ixy1 * (1.0 / 7.0);
		  vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
		  gx1 = fract(gx1);
		  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
		  vec4 sz1 = step(gz1, vec4(0.0));
		  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
		  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

		  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
		  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
		  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
		  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
		  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
		  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
		  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
		  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

		  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
		  g000 *= norm0.x;
		  g010 *= norm0.y;
		  g100 *= norm0.z;
		  g110 *= norm0.w;
		  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
		  g001 *= norm1.x;
		  g011 *= norm1.y;
		  g101 *= norm1.z;
		  g111 *= norm1.w;

		  float n000 = dot(g000, Pf0);
		  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
		  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
		  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
		  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
		  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
		  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
		  float n111 = dot(g111, Pf1);

		  vec3 fade_xyz = fade(Pf0);
		  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
		  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
		  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x); 
		  return 2.2 * n_xyz;
		}

		vec3 rgb2hsv(vec3 c) {
		    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
		    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
		    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

		    float d = q.x - min(q.w, q.y);
		    float e = 1.0e-10;
		    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
		}

		vec3 hsv2rgb(vec3 c) {
		    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
		    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
		    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
		}

		void main() {
		    float y = -(vTextureCoord.y+u_translation);
		    if(y > 1.0)
		        y = y-1.0;
		        
		    float p = cnoise(vec3(vTextureCoord.x, y, v_random)*10.0);
		    
		    vec4 color = texture2D(u_water, (vec2(vTextureCoord.x/2.0+0.5, y)));
		    color.b = color.b + color.b*p/5.0;
		    
		    vec3 hsv = rgb2hsv(color.rgb);
		    
		    hsv.z = 0.35 - v_depth*u_depth_max/u_depth_current/5.5;
		    vec3 rgb = hsv2rgb(hsv);
		    
		    float ex = 0.1+1.0/exp(pow(v_far+1.0, 15.0));

			gl_FragColor = vec4(rgb.rgb, ex);
		}
	)";

	OceanLayer(const GLFWvidmode * mode);

	void setSettings();

	void setDataLocations();

	void setData();

	void renderOcean();

	void freeOcean();
};

#endif