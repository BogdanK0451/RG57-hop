#include <float.h>
#include <math.h>
#include <GL/glut.h>
#include <string.h>


#include "vecops.h"
#include "draw.h"
#include "stdio.h"
#include "input.h"
#include "loader.h"
#include "camera.h"



 // globalna struktura, sve inicijalizovano na 0

void draw_loaded_obj()
{
	for (int j = 0; j < objNum; j++)
	{
		if (obj[j].faces != NULL)
		{
			int texId = -1;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			for (int k = 0; k < texNum; k++)	//proveravamo da li postoji tekstura ciji je naziv isti kao naziv modela
			{									//		printf("%s %s %d\n", obj[j].name, tex[k].name, j);
				if (strstr(obj[j].name, tex[k].name))
				{
					texId = k;
					break;
				}
			}																			
			draw_skybox();
			if (texId >= 0)
			{
				glEnable(GL_LIGHTING);
				int i = 0;
				while (1)
				{
					glBindTexture(GL_TEXTURE_2D, tex[texId].id);
					glBegin(GL_POLYGON);
					while (obj[j].faces[i].indexV != 0)
					{
						glTexCoord2f(obj[j].vt[obj[j].faces[i].indexVt].x,
							obj[j].vt[obj[j].faces[i].indexVt].y);
						glNormal3f(obj[j].vn[obj[j].faces[i].indexVn].x,
							obj[j].vn[obj[j].faces[i].indexVn].y,
							obj[j].vn[obj[j].faces[i].indexVn].z);
						glVertex3f(obj[j].v[obj[j].faces[i].indexV].x + obj[j].pos.x,
							obj[j].v[obj[j].faces[i].indexV].y + obj[j].pos.y,
							obj[j].v[obj[j].faces[i].indexV].z + obj[j].pos.z);
						i++;
					}
					glEnd();
					glBindTexture(GL_TEXTURE_2D, 0);
					i++;
					if (i == obj[j].count[3])
						break;
				}
			}
			//iscrtavanje objekta kada nema teksture za njega
			else
			{
				glDisable(GL_LIGHTING);
				int i = 0;
				while (1)
				{
					glBegin(GL_POLYGON);
					while (obj[j].faces[i].indexV != 0)
					{
						glNormal3f(obj[j].vn[obj[j].faces[i].indexVn].x,
							obj[j].vn[obj[j].faces[i].indexVn].y,
							obj[j].vn[obj[j].faces[i].indexVn].z);
						glVertex3f(obj[j].v[obj[j].faces[i].indexV].x + obj[j].pos.x,
							obj[j].v[obj[j].faces[i].indexV].y + obj[j].pos.y,
							obj[j].v[obj[j].faces[i].indexV].z + obj[j].pos.z);
						i++;
					}
					glEnd();
					i++;
					if (i == obj[j].count[3])
						break;
				}
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			if (SHOW_BOUNDARIES)
				draw_bounding_box(obj[j]);
		}
		else
		{
			printf(" object %d cannot be displayed", objNum);
		}
	}
}



void set_starting_bounding_box(Object *obj)
{
	obj->minX = FLT_MAX;
	obj->minY = FLT_MAX;
	obj->minZ = FLT_MAX;
	obj->maxX = FLT_MIN;
	obj->maxY = FLT_MIN;
	obj->maxZ = FLT_MIN;
}

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

//losa postavljena orijentacija, ali ce bounding boxovi uvek biti u polygonMod-u GL_LINE tako da nije bitno
void draw_bounding_box(Object obj)
{

	glPushMatrix();
	glBegin(GL_QUADS);
		glVertex3f(obj.minX, obj.minY, obj.minZ);
		glVertex3f(obj.minX, obj.minY, obj.maxZ);
		glVertex3f(obj.minX, obj.maxY, obj.maxZ);
		glVertex3f(obj.minX, obj.maxY, obj.minZ);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(obj.minX, obj.minY, obj.minZ);
		glVertex3f(obj.maxX, obj.minY, obj.minZ);
		glVertex3f(obj.maxX, obj.maxY, obj.minZ);
		glVertex3f(obj.minX, obj.maxY, obj.minZ);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(obj.minX, obj.minY, obj.maxZ);
		glVertex3f(obj.maxX, obj.minY, obj.maxZ);
		glVertex3f(obj.maxX, obj.maxY, obj.maxZ);
		glVertex3f(obj.minX, obj.maxY, obj.maxZ);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(obj.minX, obj.maxY, obj.minZ);
		glVertex3f(obj.minX, obj.maxY, obj.maxZ);
		glVertex3f(obj.maxX, obj.maxY, obj.maxZ);
		glVertex3f(obj.maxX, obj.maxY, obj.minZ);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(obj.maxX, obj.minY, obj.minZ);
		glVertex3f(obj.maxX, obj.minY, obj.maxZ);
		glVertex3f(obj.maxX, obj.maxY, obj.maxZ);
		glVertex3f(obj.maxX, obj.maxY, obj.minZ);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(obj.minX, obj.minY, obj.minZ);
		glVertex3f(obj.minX, obj.minY, obj.maxZ);
		glVertex3f(obj.maxX, obj.minY, obj.maxZ);
		glVertex3f(obj.maxX, obj.minY, obj.minZ);
	glEnd();
	glPopMatrix();


}

void draw_skybox() 
{
	glPushMatrix();
	//pomeramo skybox zajedno sa kamerom
	glTranslatef(cam.pos.x, cam.pos.y, cam.pos.z);
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
	glPopMatrix();
}

