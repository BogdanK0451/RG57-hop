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
#define TIMER_INTERVAL 16
#define NEAR_PLANE 0.1f
#define FAR_PLANE 1024


int windowWidth, windowHeight;

float angle = 0.0f;
int seconds = 0;
extern int tick = 0;

void on_reshape(int width, int height);
void on_display(void);
void on_timer(int value);
void set_scene();
void set_callbacks();

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
	glEnable(GL_LIGHTING);
	for (int i = 0; i < MAX_LIGHTS; i++) 
			glEnable(lights[i]);

	glutSetCursor(GLUT_CURSOR_NONE);

	//registracija funkcija za dogadjaje
	set_callbacks();
	
	//ucitavanje mape modela i tekstura
	load_all_files();

	glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);

	glutMainLoop();

	return 0;
}



void on_timer(int value)
{
	if (value != TIMER_ID)
		return;

	tick++;
	/*svake sekunde smanjujemo cooldown na kretanja teleport i blink*/
	if (tick % 60 == 0)
	{
		seconds++;
		if(cam.spellCooldown>0)
			cam.spellCooldown--;
	}
	/*svakih 10 sekundi dajemo igracu 1 sekundu koriscenja jetPack kretanja*/
	if (tick % 600 == 0)
		if(cam.powerCharge<120)
			cam.powerCharge += 60;

	angle = angle + 1.0f;
	
	if (tick % 300 == 0)
	{
		printf("--Stanje programa:--\n");
		printf("CAMPOS: %.1f/%.1f/%.1f \n",cam.pos.x, cam.pos.y, cam.pos.z);
		printf("STATE: MULTIPLE VIEWPORTS %d / TEXTURELESS MODE %d / NO CLIP %d\nSTATE: DAY %d / SHOW BOUNDING BOXES %d / RENDER MODE %d\n\n",
			MULTIPLE_VIEWPORTS,TEXTURELESS_MODE,DAY,NO_CLIP,SHOW_BOUNDARIES,RENDER_MODE);
		printf("--------KRAJ--------\n");
	}

	if (cam.pos.y < -50 && !NO_CLIP) {
		cam.health = 0;
		cam.pos.y = -50;
	}

	if (cam.health > 0)
	{
		cam_control(pitch, yaw);
		mov_control(tick);
	}

	glutPostRedisplay();
	glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
}


void on_reshape(int width, int height)
{
	windowWidth = width;
	windowHeight = height;
}

void on_display(void)
{
	glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (!MULTIPLE_VIEWPORTS)
	{
		glViewport(0, 0, windowWidth, windowHeight);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(
			cam.fov,
			windowWidth / (float)windowHeight,
			NEAR_PLANE, FAR_PLANE);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		if (cam.health > 0)
			adjust_cam();
		else if (cam.health <= 0)
			death_cam();
	
		
		glPushMatrix();
		set_scene();
		glPopMatrix();
		draw_ui(windowWidth, windowHeight);
		glutSwapBuffers();
	}
	else if (MULTIPLE_VIEWPORTS)
	{
		glViewport(0, 0, windowWidth/2, windowHeight/2);
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(
			cam.fov,
			windowWidth / (float)windowHeight,
			NEAR_PLANE, FAR_PLANE);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		RENDER_MODE = 2;
		if (cam.health > 0)
			adjust_cam();
		else if (cam.health <= 0)
			death_cam();
		set_scene();
		draw_ui(windowWidth/2, windowHeight/2);

	
		glViewport(windowWidth / 2, windowHeight / 2, windowWidth, windowHeight);
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(
			cam.fov,
			windowWidth / (float)windowHeight,
			NEAR_PLANE, FAR_PLANE);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		if (cam.health > 0)
			adjust_cam();
		else if (cam.health <= 0)
			death_cam();
		RENDER_MODE = 1;
		draw_loaded_obj(angle,tick);

		glutSwapBuffers();
	}
}

void set_scene()
{
	draw_light(angle);
	draw_loaded_obj(angle,tick);
	//draw_coordinate_system(); nije potrebno u finalnom programu
	glPushMatrix();
	draw_skybox();
	glPopMatrix();
}

void set_callbacks()
{
	glutSpecialFunc(on_special_keyboard);
	glutSpecialUpFunc(on_special_keyboard_up);
	glutKeyboardFunc(on_keyboard);
	glutKeyboardUpFunc(on_keyboard_up);
	glutReshapeFunc(on_reshape);
	glutDisplayFunc(on_display);
	glutPassiveMotionFunc(on_mouse_mov);
	glutMotionFunc(on_mouse_mov_and_click);
	glutMouseFunc(on_mouse_click);
}