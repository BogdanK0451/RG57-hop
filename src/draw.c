#include <float.h> // na windowsu su FLT_MAX se nalazi ovde
#include <math.h>	// na linux-u FLT_MAX se nalazi ovde
#include <GL/glut.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "vecops.h"
#include "draw.h"
#include "input.h"
#include "loader.h"
#include "camera.h"
#include "collision.h"

int effect1Speed = 1; // pocetna brzina kojom iscrtavamo objekte u efektu  https://gyazo.com/2edca5e51dff8fee23fc9e9ce9d2d948

static int polysToDraw = 0;

int lights[] = {GL_LIGHT0,GL_LIGHT1};

void draw_loaded_obj(float angle, int tick)
{
	glPushMatrix();
	for (int j = 0; j < objNum; j++)
	{
		Vec3 translation = { 0,0,0 };
		polysToDraw = polygon_draw_limit(j);
		if (obj[j].faces != NULL)
		{
			int texId = -1;
				
			draw_mode();	//GL_POINTS || GL_LINES || GL_FILL

			if(!TEXTURELESS_MODE)
			for (int k = 0; k < texNum; k++)	//proveravamo da li postoji tekstura ciji je naziv isti kao naziv modela
			{		
				if (strstr(obj[j].name, tex[k].name))
				{
					texId = k;
					break;
				}
			}		

			if (strstr(obj[j].name, OBJ_TO_USE_EFFECT1_ON))
			{
				glPushMatrix();
				glTranslatef(12, 2, 60);
				glRotatef(angle, 0, 1, 0);
				glTranslatef(-12, -2, -60);
			}

			if (strstr(obj[j].name, "deadly"))
			{
				glPushMatrix();
				translation = animate_cube(j);
			}
			/*iscravanje objekta kad ima teksture za njega*/
			if (texId >= 0)
			{	/*bojimo objekat sa kojim se dodirujemo*/
				if (obj[j].collided == 1)
				{
					glPushAttrib(GL_LIGHTING_BIT); // funkcionise po istom principu kao glPushMatrix
					glEnable(GL_COLOR_MATERIAL);
					glColor3f(0.1f, 0.0f, 0.8f);
					glDisable(GL_COLOR_MATERIAL);
				}
				/*primenjujemo specijalni materijal na objekat imena snow*/
				if (strstr(obj[j].name, "snow"))
				{
					glPushAttrib(GL_LIGHTING_BIT);
					GLfloat ambientCoeffsRed[] = { 1.0f, 0.1f, 0.1f, 1.0f };
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientCoeffsRed);
				}

				int i = 0;

				while (1)
				{
					glColor3f(1.0f, 1.0f, 1.0f);

					glBindTexture(GL_TEXTURE_2D, tex[texId].id);
					glBegin(GL_POLYGON);
						while (obj[j].faces[i].indexV != 0)
						{
							if (strstr(obj[j].name, OBJ_TO_USE_EFFECT1_ON) || strstr(obj[j].name, OBJ2_TO_USE_EFFECT1_ON))
							{
								if (i < polysToDraw)
									draw_with_texture(i, j);
							}
							else
							{
								draw_with_texture(i, j);
							}
							i++;
						}
					glEnd();
					glBindTexture(GL_TEXTURE_2D, 0);
					i++;
					if (i == obj[j].count[3])
						break;
				}
				glPopAttrib(); // skida poslednji atribut sa steka, u ovom slucaju LIGHTING
				glPopAttrib();
				glPopMatrix();
				glPopMatrix();
			}
			/*iscrtavanje objekta kada nema teksture za njega*/
			else  			
			{
				glColor3f(1.0f, 1.0f, 1.0f);
				if (obj[j].collided == 1)
					glColor3f(0.96078f, 0.08235f, 0.03922f);

				glDisable(GL_LIGHTING);
				int i = 0;
				while (1)
				{
					glBegin(GL_POLYGON);
					while (obj[j].faces[i].indexV != 0)
					{
						draw_without_texture(i, j);
						i++;
					}
					glEnd();
					i++;

					if (i == obj[j].count[3])
						break;
				}
				glPopMatrix();
				glPopMatrix();
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			if (SHOW_BOUNDARIES)	//iscrtavanje wiredcubea okolo poligona koji se koristi za koliziju, ukoliko je pritisnuto F9
				draw_bounding_box(obj[j],translation);
		}
		else
		{
			printf(" object %d cannot be displayed", objNum);
		}
	}
	glPopMatrix();
}
/*efekat https://gyazo.com/2edca5e51dff8fee23fc9e9ce9d2d948 koji se postize time sto ne iscrtavamo sve pljosni/faces nego poligone do indeksa polysToDraw*/
int polygon_draw_limit(int j)
{												
	static int polysToDraw = 0;	
	if (strstr(obj[j].name, OBJ_TO_USE_EFFECT1_ON))
	{
		if (polysToDraw < obj[j].count[3])
			polysToDraw += effect1Speed;
		else
			polysToDraw = 0;
	}
	if (polysToDraw < 0)
		polysToDraw = 0;

	return polysToDraw;
}
/*animiramo ucitane objekte,osim "vizuelnog" pomeranja istovremeno
pomeramo i njihove podatke za koliziju i bounding box*/
Vec3 animate_cube(int j) 
{
	static float movXLimit = 21.0f;
	static Vec3 mov = { 0.0f,0.0f,0.0f };
	static int sign = 1;

	mov.x += 0.05f*sign;
	if (mov.x > movXLimit)
		sign *= -1;
	if(mov.x<0)
		sign *= -1;

	update_obj_center(mov, j); //koristimo ga za koliziju, moramo da ga pomerimo, inace ce vizuelno doci do kolizije a logicki ne
	glPushMatrix();

	glTranslatef(mov.x, mov.y, mov.z);
	return mov;
}

void draw_with_texture(int i, int j)
{
	glTexCoord2f(obj[j].vt[obj[j].faces[i].indexVt].x,
		obj[j].vt[obj[j].faces[i].indexVt].y);
	glNormal3f(obj[j].vn[obj[j].faces[i].indexVn].x,
		obj[j].vn[obj[j].faces[i].indexVn].y,
		obj[j].vn[obj[j].faces[i].indexVn].z);
	glVertex3f(obj[j].v[obj[j].faces[i].indexV].x + obj[j].pos.x,
		obj[j].v[obj[j].faces[i].indexV].y + obj[j].pos.y,
		obj[j].v[obj[j].faces[i].indexV].z + obj[j].pos.z);
}

void draw_without_texture(int i, int j)
{
	glNormal3f(obj[j].vn[obj[j].faces[i].indexVn].x,
		obj[j].vn[obj[j].faces[i].indexVn].y,
		obj[j].vn[obj[j].faces[i].indexVn].z);
	glVertex3f(obj[j].v[obj[j].faces[i].indexV].x + obj[j].pos.x,
		obj[j].v[obj[j].faces[i].indexV].y + obj[j].pos.y,
		obj[j].v[obj[j].faces[i].indexV].z + obj[j].pos.z);
}

void draw_bounding_box(Object obj, Vec3 mov)
{
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS);
		glVertex3f(obj.minX + mov.x, obj.minY + mov.y, obj.minZ + mov.z);
		glVertex3f(obj.minX + mov.x, obj.minY + mov.y, obj.maxZ + mov.z);
		glVertex3f(obj.minX + mov.x, obj.maxY + mov.y, obj.maxZ + mov.z);
		glVertex3f(obj.minX + mov.x, obj.maxY + mov.y, obj.minZ + mov.z);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(obj.minX + mov.x, obj.minY + mov.y, obj.minZ + mov.z);
		glVertex3f(obj.maxX + mov.x, obj.minY + mov.y, obj.minZ + mov.z);
		glVertex3f(obj.maxX + mov.x, obj.maxY + mov.y, obj.minZ + mov.z);
		glVertex3f(obj.minX + mov.x, obj.maxY + mov.y, obj.minZ + mov.z);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(obj.minX + mov.x, obj.minY + mov.y, obj.maxZ + mov.z);
		glVertex3f(obj.maxX + mov.x, obj.minY + mov.y, obj.maxZ + mov.z);
		glVertex3f(obj.maxX + mov.x, obj.maxY + mov.y, obj.maxZ + mov.z);
		glVertex3f(obj.minX + mov.x, obj.maxY + mov.y, obj.maxZ + mov.z);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(obj.minX + mov.x, obj.maxY + mov.y, obj.minZ + mov.z);
		glVertex3f(obj.minX + mov.x, obj.maxY + mov.y, obj.maxZ + mov.z);
		glVertex3f(obj.maxX + mov.x, obj.maxY + mov.y, obj.maxZ + mov.z);
		glVertex3f(obj.maxX + mov.x, obj.maxY + mov.y, obj.minZ + mov.z);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(obj.maxX + mov.x, obj.minY + mov.y, obj.minZ + mov.z);
		glVertex3f(obj.maxX + mov.x, obj.minY + mov.y, obj.maxZ + mov.z);
		glVertex3f(obj.maxX + mov.x, obj.maxY + mov.y, obj.maxZ + mov.z);
		glVertex3f(obj.maxX + mov.x, obj.maxY + mov.y, obj.minZ + mov.z);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(obj.minX + mov.x, obj.minY + mov.y, obj.minZ + mov.z);
		glVertex3f(obj.minX + mov.x, obj.minY + mov.y, obj.maxZ + mov.z);
		glVertex3f(obj.maxX + mov.x, obj.minY + mov.y, obj.maxZ + mov.z);
		glVertex3f(obj.maxX + mov.x, obj.minY + mov.y, obj.minZ + mov.z);
	glEnd();
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_skybox() 
{
	glPushMatrix();

	glEnable(GL_LIGHTING);
	glTranslatef(cam.pos.x, cam.pos.y, cam.pos.z);		//pomeramo skybox zajedno sa kamerom
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glScalef(500, 500, 500);
	
	glBindTexture(GL_TEXTURE_2D, skybox[0]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(1.0f, -1.0f, -1.0f);
		glTexCoord2f(1, 0); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1, 1); glVertex3f(-1.0f, 1.0f, -1.0f);
		glTexCoord2f(0, 1); glVertex3f(1.0f, 1.0f, -1.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, skybox[3]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(1.0f, -1.0f, 1.0f);
		glTexCoord2f(1, 0); glVertex3f(1.0f, -1.0f, -1.0f);
		glTexCoord2f(1, 1); glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f(0, 1); glVertex3f(1.0f, 1.0f, 1.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, skybox[2]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-1.0f, -1.0f, 1.0f);
		glTexCoord2f(1, 0); glVertex3f(1.0f, -1.0f, 1.0f);
		glTexCoord2f(1, 1); glVertex3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(0, 1); glVertex3f(-1.0f, 1.0f, 1.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, skybox[1]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1, 0); glVertex3f(-1.0f, -1.0f, 1.0f);
		glTexCoord2f(1, 1); glVertex3f(-1.0f, 1.0f, 1.0f);
		glTexCoord2f(0, 1); glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, skybox[4]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 1); glVertex3f(-1.0f, 1.0f, -1.0f);
		glTexCoord2f(0, 0); glVertex3f(-1.0f, 1.0f, 1.0f);
		glTexCoord2f(1, 0); glVertex3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(1, 1); glVertex3f(1.0f, 1.0f, -1.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, skybox[5]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(0, 1); glVertex3f(-1.0f, -1.0f, 1.0f);
		glTexCoord2f(1, 1); glVertex3f(1.0f, -1.0f, 1.0f);
		glTexCoord2f(1, 0); glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glDisable(GL_LIGHTING);

	glPopMatrix();
}
/*postavljamo matricu projekcije na ortogonalnu i iscrtavamo ui elemente*/
void draw_ui(int windowWidth, int windowHeight)
{
	glPushMatrix();
	glDisable(GL_LIGHTING);
	
	glViewport(0, 0, windowWidth, windowHeight);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, windowWidth, windowHeight, 0, 0, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1, 1, 1);
	glLineWidth(1.0);
	glPointSize(2);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(1.0f, 0.9569f, 0.3098f);
	/*iscrtavanje kruznog nisana sa tackom u centru*/
	glBegin(GL_POINTS);
		glVertex2f((GLfloat)windowWidth / 2, (GLfloat)windowHeight / 2);
	glEnd();								

	int radius = 15;

	glBegin(GL_POLYGON);
		for (int i = 0; i <= 360.0f; i++)
		{
			float angle = 2.0f * (float)PI * (float)i / 360.0f;
			float x = (float)cos(angle);
			float y = (float)sin(angle);
			glVertex2f(windowWidth / 2 + x * radius, windowHeight / 2 + y * radius);
		}
	glEnd();
	/*iscrtavanje plavog kruga koji nas informise da ne mozemo vise da letimo iliti kad je cam.powerCharge==0*/
	if (!cam.powerCharge)			
	{
		radius = 17;
		glLineWidth(2);
		glColor3f(0.0f, 0.0f, 1.0f);
		glBegin(GL_POLYGON);

		for (int i = 0; i <= 360.0f; i++)
		{
			float angle = 2.0f * (float)PI * (float)i / 360.0f;
			float x = (float)cos(angle);
			float y = (float)sin(angle);
			glVertex2f(windowWidth / 2 + x * radius, windowHeight / 2 + y * radius);
		}
		glEnd();
	}
	//iscrtavanje crvenog kruga kada imamo cooldown na teleport ili blink */
	if (cam.spellCooldown)
	{
		radius = 16;
		glLineWidth(5);
		glColor3f(0.96078f, 0.08235f, 0.03922f);
		glBegin(GL_POLYGON);

		for (int i = 0; i <= 360.0f; i++)
		{
			float angle = 2.0f * (float)PI * (float)i / 360.0f;
			float x = (float)cos(angle);
			float y = (float)sin(angle);
			glVertex2f(windowWidth / 2 + x * radius, windowHeight / 2 + y * radius);
		}
		glEnd();
	}
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_coordinate_system()
{
	glPushMatrix();
	glLineWidth(3);
	glDisable(GL_LIGHTING);
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
	glEnable(GL_LIGHTING);
}

void draw_light(float angle) 
{
	static	GLfloat light0Position[] = { 12.0f, 9.0f, 60.0f, 1.0f };
	static	GLfloat light0Ambient[] = { 0.7f, 0.2f, 0.2f, 1.0f };
	static	GLfloat light0Diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	static	GLfloat light0Specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	static  GLfloat light1Position[] = { 1000,1000,1000,0 };		//"global light"
	static	GLfloat light1Ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static	GLfloat light1Diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	static	GLfloat light1Specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	static	GLfloat ambientCoeffs[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static	GLfloat diffuseCoeffs[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static	GLfloat specularCoeffs[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	GLfloat shininess = 20;

	glShadeModel(GL_SMOOTH);
	glPushMatrix();
		glTranslatef(8.0f, 9.0f, 60.0f);
		glRotatef(angle, 0, 1, 0);
		glTranslatef(-8.0f,-9.0f,-60.0f);
		glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light0Ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light0Specular);
	glPopMatrix();
	/*promena globalnog osvetljanja koji daju efekat noci*/
	if (DAY == 0)
	{
		if (light1Ambient[0] > 0.0f)
		{
			light1Ambient[0] -= 0.005f;
			light1Ambient[1] -= 0.005f;
			light1Ambient[2] -= 0.005f;
		}
	
	}
	/*promena globalnog osvetljanja koji daju efekat dana*/
	else if (DAY == 1)
	{
		if (light1Ambient[0] < 1.0f)
		{
			light1Ambient[0] += 0.005f;
			light1Ambient[1] += 0.005f;
			light1Ambient[2] += 0.005f;
		}
	}
		
	glLightfv(GL_LIGHT1, GL_POSITION, light1Position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1Ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1Specular);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientCoeffs);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseCoeffs);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularCoeffs);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.3f);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01f);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f);
}
/*poligon mode POINTS LINES ILI FILL*/
void draw_mode()
{
	switch (RENDER_MODE) 
	{
		case 0:
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			glPointSize(3);
			break;
		case 1:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
			break;
		case 2:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
	}
}
/*postavljamo pocetne vrednosti za bounding box,
pre nego sto je objekat jos ucitan iz .obj fajla*/
void set_starting_bounding_box(Object *obj)
{
	obj->minX = FLT_MAX;
	obj->minY = FLT_MAX;
	obj->minZ = FLT_MAX;
	obj->maxX = FLT_MIN;
	obj->maxY = FLT_MIN;
	obj->maxZ = FLT_MIN;
}
/*menjamo minimalne i maksimalne koordinate bounding box-a
tokom ucitavanja modela iz .obj fajla*/
void update_obj_min_max(Object * obj, float comparatorX, float comparatorY, float comparatorZ)
{
	if (comparatorX < obj->minX)
		obj->minX = comparatorX;
	if (comparatorY < obj->minY)
		obj->minY = comparatorY;
	if (comparatorZ < obj->minZ)
		obj->minZ = comparatorZ;
	if (comparatorX > obj->maxX)
		obj->maxX = comparatorX;
	if (comparatorY > obj->maxY)
		obj->maxY = comparatorY;
	if (comparatorZ > obj->maxZ)
		obj->maxZ = comparatorZ;
}

void update_obj_dimensions(Object * obj)
{
	obj->centerX = (obj->minX + obj->maxX) / 2;
	obj->centerY = (obj->minY + obj->maxY) / 2;
	obj->centerZ = (obj->minZ + obj->maxZ) / 2;

	obj->width = (float)fabs(obj->maxX - obj->minX);
	obj->height = (float)fabs(obj->maxY - obj->minY);
	obj->depth = (float)fabs(obj->maxZ - obj->minZ);
}
/*menjamo koordinate centra objekta za vektor translate*/
void update_obj_center(Vec3 translate, int j)
{
	obj[j].centerX = translate.x + obj[j].pos.x;
	obj[j].centerY = translate.y + obj[j].pos.y;
	obj[j].centerZ = translate.z + obj[j].pos.z;

	collision_on_obj_mov(); //proveravamo koliziju pokretnog objekta sa igracem
}
