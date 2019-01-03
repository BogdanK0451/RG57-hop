#include <GL/glut.h>

#include<stdio.h>

#include "input.h"
#include "camera.h"
#include "loader.h"
#define WINDOW_WIDTH 1650
#define WINDOW_HEIGHT 1050


float lastX = WINDOW_WIDTH / 2;
float lastY = WINDOW_HEIGHT / 2;

GLfloat pitch = 0;
GLfloat yaw = 0;
//placeholder
GLfloat roll = 0;

static int KEY_W = 0;
static int KEY_A = 0;
static int KEY_S = 0;
static int KEY_D = 0;
static int KEY_SPACE = 0;
static int KEY_F11 = 0;
static int LEFT_ARROW = 0;
static int RIGHT_ARROW = 0;
static int UP_ARROW = 0;
static int DOWN_ARROW = 0;

void call_movement_func()
{
	if (KEY_W)
		mov_forw(0.25f);
	if (KEY_S)
		mov_forw(-0.25f);
	if (KEY_A)
		mov_right(-0.25f);
	if (KEY_D)
		mov_right(0.25f);
	if (KEY_SPACE)
		mov_up(0.25f);
	if(UP_ARROW)
		rot_cam(pitch, yaw);
	if(DOWN_ARROW)
		rot_cam(pitch, yaw);
	if(LEFT_ARROW)
		rot_cam(pitch, yaw);
	if(RIGHT_ARROW)
		rot_cam(pitch, yaw);
}

void check_if_pitch_89()
{
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
}



void on_keyboard(unsigned char key, int x, int y)
{
	switch (key) 
	{
	case 27:

		//oslobadanje alocirane memorije
		free(v);
		free(vt);
		free(vn);
		free(faces);
		exit(EXIT_SUCCESS);
		break;
	case 'W':
	case 'w':
		KEY_W = 1;
		break;
	case 'S':
	case 's':
		KEY_S = 1;
		break;
	case 'D':
	case 'd':
		KEY_D = 1;
		break;
	case 'A':
	case 'a':
		KEY_A = 1;
		break;
	case 32:
		KEY_SPACE = 1;
		break;
	}
	

}

void on_keyboard_up(unsigned char key, int x, int y) 
{
	switch (key)
	{
	case 'W':
	case 'w':
		KEY_W = 0;
		break;
	case 'S':
	case 's':
		KEY_S = 0;
		break;
	case 'D':
	case 'd':
		KEY_D = 0;
		break;
	case 'A':
	case 'a':
		KEY_A = 0;
		break;
	case 32:
		KEY_SPACE = 0;
		break;
	}

}


void on_mouse_mov(int xPos, int yPos)
{
	int xCenter = glutGet(GLUT_WINDOW_WIDTH) / 2;
	int yCenter = glutGet(GLUT_WINDOW_HEIGHT) / 2;

	static int warped = 0;

	//postavljanje misa na poziciju glutWarpPointerom-om poziva callback funkciju on_mouse_mov, sto sprecavamo dole navedenim uslovom
	if (warped)
	{
		warped = 0;
		return;
	}

	warped = 1;
	//pri prebrzom kretanju mis iskace iz window-a
	glutWarpPointer(xCenter, yCenter);

	float xOffset = (float)xPos - xCenter;
	float yOffset = (float)yPos - yCenter;


	float sensitivity = 0.10f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;

	check_if_pitch_89();

}


void on_special_keyboard_down(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_RIGHT:
		yaw += 5.f;
		RIGHT_ARROW = 1;
		break;
	case GLUT_KEY_LEFT:
		yaw += -5.f;
		LEFT_ARROW = 1;
		break;
	case GLUT_KEY_UP:
		pitch += 5.f;
		check_if_pitch_89();
		UP_ARROW = 1;
		break;
	case GLUT_KEY_DOWN:
		pitch += -5.f;
		check_if_pitch_89();
		DOWN_ARROW = 1;
		break;
	case GLUT_KEY_F11:
		if (!KEY_F11)
		{
			glutFullScreen();
			KEY_F11 = 1;
		}
		else {
			glutPositionWindow(100, 0);
			KEY_F11 = 0;
		}
	}
}
void on_special_keyboard_up(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_RIGHT:
		RIGHT_ARROW = 0;
		break;
	case GLUT_KEY_LEFT:
		LEFT_ARROW = 0;
		break;
	case GLUT_KEY_UP:
		UP_ARROW = 0;
		break;
	case GLUT_KEY_DOWN:
		DOWN_ARROW = 0;
		break;
	}
}
