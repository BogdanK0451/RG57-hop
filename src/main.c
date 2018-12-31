
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <string.h>

#include "input.h"
#include "camera.h"


#define TIMER_ID 0
#define TIMER_INTERVAL 16.66
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 800

typedef struct vertexes {
	float x;
	float y;
	float z;
} Model;


int windowWidth, windowHeight;

float angle = 0;

Model vxs[13900];
float faces[302][6];

void on_reshape(int width, int height);
void on_display(void);
void on_timer(int value);

void load_model(char* path);

void let_there_be_light();
void draw_ui();
void generate_gallery();
void generate_spinning_cube();



void generate_plank(int length);




int main(int argc, char *argv[])
{

	/*Vrsimo inicijalizaciju osnovnih funkcionalnosti*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);
	glutInitWindowPosition(100, 0);
	glutCreateWindow("Hop");

	glEnable(GL_DEPTH_TEST);
	glutSetCursor(GLUT_CURSOR_NONE);


	let_there_be_light();


	//registracija funkcija za dogadjaje
	glutSpecialFunc(on_special_keyboard_in);
	glutKeyboardFunc(on_keyboard);
	glutReshapeFunc(on_reshape);
	glutDisplayFunc(on_display);
	glutPassiveMotionFunc(on_mouse_mov);

	
	glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);

	load_model("src/Text.obj");

	/*petlja koja slusa i poziva metode za dogadjaje*/
	glutMainLoop();

	return 0;
}



void on_timer(int value) {
	if (value != TIMER_ID)
		return;

	angle = angle + 1;

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
		1, 100);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	adjust_camera();
	
	//generate_plank(1);

	glColor3f(0.5, 0.5, 0.5);
	//generate_gallery();
	glPushMatrix();
	glScalef(20, 20, 20);
	glRotatef(angle, 0, 1, 0);
	glPointSize(3);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	for (int j = 0; j < 13900; j = j + 1) {
		glBegin(GL_TRIANGLES);
		glVertex3f(vxs[j].x, vxs[j].y, vxs[j].z);
		glVertex3f(vxs[j + 1].x, vxs[j + 1].y, vxs[j + 1].z);
		glVertex3f(vxs[j + 2].x, vxs[j + 2].y, vxs[j + 2].z);

	//printf("%f %f %f\n %f %f %f \n %f %f %f\n",vxs[j].x,vxs[j].y,vxs[j].z,vxs[j+1].x,vxs[j+1].y,vxs[j+1].z,vxs[j+2].x,vxs[j+2].y,vxs[j+2].z);
		glEnd();
	}
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

void generate_plank(int length) {
	for (int i = 0; i < length; i++) {
		glColor3f(0, 1, 1);
		glutSolidCube(2);
		glPushMatrix();
		glColor3f(1, 1, 0);
		glTranslatef(0, 0, 2);
		glutSolidCube(2);
		glPopMatrix();
		glTranslatef(2, 0, 0);
	}


}

void load_model(char* path) {

	FILE * model = fopen(path, "r");

	if (model == NULL) {
		fprintf(stderr, " Error opening file: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	int i = 0;

	while (1) {
		char buf[128];

	//	int v1 = fscanf(model, "%s", buf);
		

		if (fgets(buf, 128, model)==NULL)
			break;
		if (buf[0] == 'v' && (buf[1] != 't' && buf[1] != 'n')) {
			sscanf(buf, "%*s %f %f %f", &vxs[i].x, &vxs[i].y, &vxs[i].z);
			//printf("%f",vxs[i].x);
			i = i + 1;
		}
	}
}

void generateArena() 
{
}

void generate_gallery()
{
	generate_spinning_cube();

}


void generate_spinning_cube() {

	glPushMatrix();
	glTranslatef(0, 0, 1);
	glRotatef(angle, 1, 0, 0);

	glLineWidth(1);
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

	glPopMatrix();
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
	GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };

	/* Ambijentalna boja svetla. */
	GLfloat light_ambient[] = { 0.0f, 0.0f, 0.7f, 1.0f };

	/* Difuzna boja svetla. */
	GLfloat light_diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };

	/* Spekularna boja svetla. */
	GLfloat light_specular[] = { 0.9f, 0.9f, 0.9f, 1.0f };

	/* Koeficijenti ambijentalne refleksije materijala. */
	GLfloat ambient_coeffs[] = { 1.0f, 0.1f, 0.1f, 1.0f };

	/* Koeficijenti difuzne refleksije materijala. */
	GLfloat diffuse_coeffs[] = { 0.0f, 0.0f, 0.8f, 1.0f };

	/* Koeficijenti spekularne refleksije materijala. */
	GLfloat specular_coeffs[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	/* Koeficijent glatkosti materijala. */
	GLfloat shininess = 20;

	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);

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



/*
		glutSolidSphere(0.5,20,20);
		GLUquadric * qobj;
		qobj= gluNewQuadric();
		gluQuadricNormals(qobj, GLU_SMOOTH);
		gluCylinder(qobj,0.5,1,1,20,20);*/
