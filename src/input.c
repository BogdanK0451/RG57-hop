#include <GL/glut.h>

#include<stdio.h>

#include "input.h"
#include "camera.h"
#define WINDOW_WIDTH 1650
#define WINDOW_HEIGHT 1050


float lastX = WINDOW_WIDTH / 2;
float lastY = WINDOW_HEIGHT / 2;

GLfloat pitch = 0;
GLfloat yaw = 0;
GLfloat roll = 0;



void on_keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 27:
			exit(EXIT_SUCCESS);
			break;
		case 'W':
		case 'w':
			mov_forw(0.25f);
			glutPostRedisplay();
			break;
		case 'S':
		case 's':
			mov_forw(-0.25f);
			glutPostRedisplay();
			break;
		case 'D':
		case 'd':
			mov_right(0.25f);
			glutPostRedisplay();
			break;
		case 'A':
		case 'a':
			mov_right(-0.25f);
			glutPostRedisplay();
			break;
		case 32:
			mov_up(0.25f);
			glutPostRedisplay();
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


	float sensitivity = 0.001f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;


	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	printf("%f%f\n", pitch, yaw);

	rot_cam(pitch, yaw);

	glutPostRedisplay();

}


void on_special_keyboard_in(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_RIGHT:
		rot_x(5.0f);
		break;
	}
}
