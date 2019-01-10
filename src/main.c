
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <string.h>

#include "input.h"
#include "camera.h"
#include "loader.h"
#include "collision.h"
#include "draw.h"


#define TIMER_ID 0
#define TIMER_INTERVAL 16.66
#define WINDOW_WIDTH 1650
#define WINDOW_HEIGHT 1050
#define NEAR_PLANE 1
#define FAR_PLANE 1024


int windowWidth, windowHeight;

float angle = 0;



void on_reshape(int width, int height);
void on_display(void);
void on_timer(int value);

void let_there_be_light();
void draw_ui();


int main(int argc, char *argv[])
{

	/*Vrsimo inicijalizaciju osnovnih funkcionalnosti*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);
	glutInitWindowPosition(100, 0);
	glutCreateWindow("Hop");

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glutSetCursor(GLUT_CURSOR_NONE);





	//registracija funkcija za dogadjaje
	glutSpecialFunc(on_special_keyboard);
	glutSpecialUpFunc(on_special_keyboard_up);
	glutKeyboardFunc(on_keyboard);
	glutKeyboardUpFunc(on_keyboard_up);
	glutReshapeFunc(on_reshape);
	glutDisplayFunc(on_display);
	glutPassiveMotionFunc(on_mouse_mov);
	
	let_there_be_light();
	load_all_files();

	
	glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);


	/*petlja koja slusa i poziva metode za dogadjaje*/
	glutMainLoop();

	return 0;
}



void on_timer(int value)
{
	if (value != TIMER_ID)
		return;



	angle = angle + 1;
	rot_cam(pitch, yaw);
	call_movement_func();

	glutPostRedisplay();

	//if (animation_ongoing)
	glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
}


void on_reshape(int width, int height)
{
	/* Pamte se sirina i visina prozora. */
	windowWidth = width;
	windowHeight = height;
}

void on_display(void)
{
	glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glViewport(0, 0, windowWidth, windowHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(
		75,
		windowWidth / (float)windowHeight,
		NEAR_PLANE, FAR_PLANE);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	adjust_camera();
	
	glColor3f(0.5, 0.5, 0.5);
	glPushMatrix();
//	glRotatef(angle, 0, 1, 0);
	glPointSize(3);

	draw_loaded_obj();
	


	glPopMatrix();

	glLineWidth(3);
	
	glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(100, 0, 0);

		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 100, 0);

		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 100);
	glEnd();


	//postavljamo matricu projekcije na ortogonalnu i iscrtavamo ui elemente
	draw_ui();

	glutSwapBuffers();
}


void draw_ui()
{
	glPushMatrix();
	glViewport(0, 0, windowWidth, windowHeight);
	glMatrixMode(GL_PROJECTION);
	
	glLoadIdentity();
	glOrtho(0,windowWidth, windowHeight, 0, 0, 1);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1, 1, 1);
	glLineWidth(1.0);
	glPointSize(2);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//iscrtavanje kruznog nisana sa tackom u centru
	glBegin(GL_POINTS);
		glVertex2f((GLfloat)windowWidth / 2, (GLfloat)windowHeight / 2);
	glEnd();

	glBegin(GL_POLYGON);
		int radius = 15;
	
		for (int i = 0; i <= 360.0f; i++)
		{
			float angle = 2.0f * (float)PI * (float)i / 360.0f;
			float x = (float)cos(angle);
			float y = (float)sin(angle);
			glVertex2f(windowWidth/2+x*radius, windowHeight/2+y*radius);
		}
	glEnd();

	
	glPopMatrix();
}

void let_there_be_light() {

	/* Pozicija svetla (u pitanju je direkcionalno svetlo). */
	GLfloat light_position[] = { 100.0f, 100.0f, 100.0f, 0.0f };

	/* Ambijentalna boja svetla. */
	GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	/* Difuzna boja svetla. */
	GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };

	/* Spekularna boja svetla. */
	GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	/* Koeficijenti ambijentalne refleksije materijala. */
	GLfloat ambient_coeffs[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	/* Koeficijenti difuzne refleksije materijala. */
	GLfloat diffuse_coeffs[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	/* Koeficijenti spekularne refleksije materijala. */
	GLfloat specular_coeffs[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	/* Koeficijent glatkosti materijala. */
	GLfloat shininess = 0;

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	/* Podesavaju se parametri materijala. */
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

}



// glPolygonMode(face ,fill lines points);
// glClipPlane(GL_CLIP_PLANE0,clip_plane);
// 	glEnable(GL_CLIP_PLANE0);
//glEnable(GL_COLOR_MATERIAL);
//glColorMaterial(frontback,ambient diffuse etc);

