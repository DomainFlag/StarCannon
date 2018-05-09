# StarCannon Project

A cross-platform game engine for Desktop/Web platform built on OpenGL/WebGL, that simulates procedural generated terrain combining game elements.

Check the github url: https://github.com/DomainFlag/StarCannon and pull the latest version possible if there is any!

![Screenshot](/Screenshots/screenshot.png?raw=true "Optional Title")

## Getting Started

The necessary libraries are GL, GLFW, SOIL, GLEW,  each should be included in the /usr/include folder, each game engine component is
executed within its folder with make command. Currently only on Linux machines it can be executed, latter cmake will be used to make it cross-platform on different OS like Windows.

Installing the SOIL library:
	sudo apt-get install libsoil-dev

Installing the freetype library:
	1. [Freetype releases](https://download.savannah.gnu.org/releases/freetype/)
	2. Get latest release, for now is the "freetype-2.9.tar.gz"
	3. Extract the archive: "tar xf 'archive name'""
	4. Preparing to build the application and move the application files when building it at the /usr/local location: ./configure --prefix=/usr/local
	5. Build the source code: make
	6. Move all the needed for the application files to the appropriate system directories /usr/local: sudo make install

Installing GLFW library:
	1. sudo apt-get install libglfw3-dev libglfw3

Installing OpenGL utilities:
	1. sudo apt-get install mesa-utils

Installing GLEW library:
	1. sudo apt-get install libglew-dev

## Structure

The main folder Starcannon contains all the necessary prerequisites for building the executable combining all engine components.

The main folder is consisted of approximative 15 programs each independent that consists of a header file, cpp file with its implementation, makefile used to generate the executable for the main program wthat corresponds to a specific engine component.

There is also a Tools folder, that contains different utilities like the Matrix & vector manipulation, Obj reader for 3D obj files, Heightmap reader for reading grayscale 3D terrain, Simplex Noise implementation for terrain generation and the Obj files and Textures, all apart.

## Miscellaneous

All the components were tested, although there are situations where the iostream and glfw library have some unresolved internal issues in their lastest versions with minor leaks of ~76000oc for iostream when it's included and ~80oc for glfw when opening up the window.

The project was managed with GIT as the main version control system(GitHuB).

## Issues and future retrospectives

There are issues with procedural terrain where during translation there are distortions as the translation occurs, different heights are generated from noise function that reveals gaps between different frames. Also the Thruster/Blaster/Spacecraft needs to be fit together relative to the world space and more bugs to be fixed.

Stay in touch, more updates and revisions will come.

## Build for Univ project with
11512312 MARCEL LUCA
11612186 CHIVRIGA Cristian