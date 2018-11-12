#include <stdio.h>
#include <GL/glut.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	/*Vrsimo inicijalizaciju osnovnih funkcionalnosti*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	/*Inicijalizacija prozora*/
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	/*Registruju se funkcije za obradu dogadjaja*/
	/* Obavlja se openGL inicijalizacija*/
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.75, 0.75, 0.75, 0);

	/* Ulazi se u glavnu petlju! */
	glutMainLoop();


	return 0;
}




