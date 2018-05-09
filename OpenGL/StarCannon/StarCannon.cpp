#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <SOIL/SOIL.h>
#include "./../Shader/Shader.h"
#include "./../Tools/ObjReader/ObjReader.h"
#include "./../Tools/Matrix/Matrix.h"
#include "./../Tools/Noise/Noise.h"

#include "./../X-Fighter/X-Fighter.h"
#include "./../Audio/Audio.h"
#include "./../Thruster/Thruster.h"
#include "./../Blaster/Blaster.h"
#include "./../Terrain/Terrain.h"
#include "./../Sun/Sun.h"
#include "./../Sky/Sky.h"
using namespace std;

const GLFWvidmode * mode;

Thruster * thruster_p;
Terrain * terrain_p;
Blaster * blaster_p;
Audio * audio_p;

string song = "start_sound_1";
string flight = "start_sound_2";
string shot = "start_sound_0";

void input(GLFWwindow * window, int key, int action, int u, int i) {
	terrain_p->keyboardListener(window, key, action, u, i);
    thruster_p->keyboardListener(window, key, action, u, i);
	// skyP->keyboardListener(window, key, action, u, i);

	// audioP->keyboardListener(window, key, action, u, i);

	switch(key) {
		case GLFW_KEY_ESCAPE : {
			glfwSetWindowShouldClose(window, true);
			break;
		};
	};
};

void cursor(GLFWwindow * window, double x, double y) {
    terrain_p->cursorListener(window, x, y);

    // vector<float> quaternionRot = fromEuler(-terrain_p->pitch/M_PI*180.0f, terrain_p->yaw/M_PI*180.0f, 0);
    // vector<float> result = transformQuat(vector<float>{0.0f, 0.0f, -terrain_p->terrain.speed}, quaternionRot);
    // blaster_p->position = addValues(blaster_p->position, result);


    // float xx = cos(terrain_p->pitch)*cos(terrain_p->yaw);
    // float yy = sin(terrain_p->pitch);
    // float zz = cos(terrain_p->pitch)*sin(terrain_p->yaw);
    // blaster_p->direction = vector<float>{xx, yy, zz};

    // skyP->cursorListener(window, x, y);
};

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        blaster_p->fireShots();

        audio_p->changeAudio(shot);
    }
}

int main(int argc, char ** argv) {
	GLFWwindow * window;

	if(!glfwInit())
		return -1;

	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	window = glfwCreateWindow(mode->width, mode->height, "StarCannon", glfwGetPrimaryMonitor(), NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	cout << glGetString(GL_VERSION) << endl;

	if(glewInit() != GLEW_OK) {
		std::cout << "error..!!" << std::endl;
	}

	X_Fighter x_fighter(mode);
	Thruster thruster(mode);
	Blaster blaster(mode);
	Terrain terrain(mode);
    Sun sun(mode);
    Sky sky(mode);
	Audio audio;

	audio.changeAudio(song);
	audio.changeAudio(flight);

    terrain_p = &terrain;
    thruster_p = &thruster;
	blaster_p = &blaster;
	audio_p = &audio;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, mode->width, mode->height);

	glfwSetKeyCallback(window, input);
	glfwSetCursorPosCallback(window, cursor);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sky.renderProgram(sun.viewMatrix.multiplyVector(sun.position), -sun.rotX, 0);

        sun.renderProgram();

		terrain.renderProgram();

		blaster.renderShots();

		x_fighter.renderProgram();

		thruster.renderProgram();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	terrain.freeProgram();

	x_fighter.freeProgram();

	thruster.freeProgram();

    sun.freeProgram();

    sky.freeProgram();

	glfwTerminate();
	return 0;
}