#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>

#include "loader.h"
#include "../external/re.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"

Vec3 *v = NULL;		/*Obj file info https://www.fileformat.info/format/wavefrontobj/egff.htm */
Vec2 *vt = NULL;
Vec3 *vn = NULL;


int components;
GLuint texture;
int tWidth, tHeight;



	
Faces *faces = NULL;
	
/* postavljamo vrednosti na 1 jer pri crtanju objekta moramo da povezemo vertexe zadate sa face-ovima 
npr. f 1 2 5 7 a neki od faceova ukljucuju vertex 1 pa ukoliko bismo kretali od 0 uvek bi se taj vertex preskocio i poligon bi pogresno bio iscrtan

*/
					//vtx txture norm   brojac za pljosni 
					//	|   |    |		/
static int count[4] = { 1,  1,   1,    0 };

void load_model(char * path)
{
	stbi_set_flip_vertically_on_load(1);
	unsigned char* data = stbi_load("resources/Stormtrooper_D.tga", &tWidth, &tHeight, &components, 3);
	if(data==NULL)
	{
		fprintf(stderr, " Loading texture, error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); 
	
	// Parametri tekstura
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);


		// pocetne velicine nizova
		int vSize = 4;
		int vtSize = 4;
		int vnSize = 4;
		int facesSize = 20;

		//dinamicka alokacija
		v = (Vec3*)malloc(sizeof(Vec3) * vSize);
		vt = (Vec2*)malloc(sizeof(Vec2) * vtSize);
		vn = (Vec3*)malloc(sizeof(Vec3) * vnSize);
		faces = (Faces*)malloc(sizeof(Faces) * facesSize);



		if (!v || !vt || !vn || !faces)
		{
			fprintf(stderr, " Error allocating memory: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		FILE * model;
		model= fopen(path, "r");

		if (model == NULL) {
			fprintf(stderr, " Error opening file: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		while (1)
		{
			char buf[256];
			if (fgets(buf, 256, model) == NULL)
				break;

			//parsiramo vertekse

			if (buf[0] == 'v' && (buf[1] != 't' && buf[1] != 'n'))
			{
				if (count[0] >= vSize - 2)
				{
					vSize = vSize * 2;
					//implementirati proveru gresaka
					//newptr= realloc(oldptr,size)
					v = (Vec3*)realloc(v, sizeof(Vec3) * vSize);
				}
				sscanf(buf, "%*s %f %f %f", &v[count[0]].x, &v[count[0]].y, &v[count[0]].z);
				count[0]++;
			}

			//parsiramo koordinate tekstura

			else if (buf[0] == 'v' && buf[1] == 't')
			{
				if (count[1] == vtSize - 2)
				{
					vtSize = vtSize * 2;
					//implementirati proveru gresaka
					vt = (Vec2*)realloc(vt, sizeof(Vec2) * vtSize);
				}
				sscanf(buf, "%*s %f %f", &vt[count[1]].x, &vt[count[1]].y);
				count[1]++;
			}

			//parsiramo normale

			else if (buf[0] == 'v' && buf[1] == 'n')
			{
				if (count[2] == vnSize - 2)
				{
					vnSize = vnSize * 2;
					
					//newptr= realloc(oldptr,size)
					vn = (Vec3*)realloc(vn, sizeof(Vec3) * vnSize);
				}
				sscanf(buf, "%*s %f %f %f", &vn[count[2]].x, &vn[count[2]].y, &vn[count[2]].z);
				count[2]++;
			}

			//parsiramo faces/pljosni

			else if (buf[0] == 'f' && (buf[1] == ' '))
			{
				char * token;

				if (count[3] >= facesSize - 4)
				{
					facesSize = facesSize * 2;
					faces = (Faces*)realloc(faces, sizeof(Faces) * facesSize);
				}

				if (strchr(buf, '/'))
				{

					if (re_match("[0-9]*/[0-9]*/[0-9]* ", buf) != -1)   //  f  x/x/x y/y/y /z/z/z
					{
						int i = 1;
						token = strtok(buf, "/ ");
						while (token != NULL)
						{
							if (strcmp(token, "f"))
							{
								if (i % 3 == 1)
								{
									faces[count[3]].indexV = atoi(token);

								}

								else if (i % 3 == 2)
								{
									faces[count[3]].indexVt = atoi(token);
								}
								else if (i % 3 == 0)
								{
									faces[count[3]].indexVn = atoi(token);
									count[3]++;
								}
								i++;
							}
							token = strtok(NULL, "/ ");
						}
					}

					else if (re_match("[0-9]*/[0-9]*", buf) != -1)   //  f  x/x y/y /z/z
					{
						int i = 1;
						token = strtok(buf, "/ ");
						while (token != NULL)
						{
							//ignorisemo token  f\0, interesuju nas samo numericke vrednosti
							if (strcmp(token, "f"))
							{
								if (i % 2 == 1)
								{
									faces[count[3]].indexV = atoi(token);

								}

								else if (i % 2 == 0)
								{
									faces[count[3]].indexVt = atoi(token);
									faces[count[3]].indexVn = 0;
									count[3]++;
								}
								i++;
							}
							token = strtok(NULL, "/ ");
						}
					}

					else if (re_match("//", buf) != -1)   //  f  x//x y//y /z//z
					{
						int i = 1;
						token = strtok(buf, "/ ");
						while (token != NULL)
						{
							if (strcmp(token, "f"))
							{
								if (i % 2 == 1)
								{
									faces[count[3]].indexV = atoi(token);

								}

								else if (i % 2 == 0)
								{
									faces[count[3]].indexVn = atoi(token);
									faces[count[3]].indexVt = 0;
									count[3]++;
								}
								i++;
							}
							token = strtok(NULL, "/ ");
						}
					}

				}
				else									// f x y z
				{
					token = strtok(buf, " ");
					while (token != NULL)
					{
						if (strcmp(token, "f"))
						{
							faces[count[3]].indexV = atoi(token);
							faces[count[3]].indexVt = 0;
							faces[count[3]].indexVn = 0;
							count[3]++;
						}
						token = strtok(NULL, " ");
					}
				}
				//razdvajanje dve pljosni u nizu nulama
				faces[count[3]].indexV = 0;
				faces[count[3]].indexVt = 0;
				faces[count[3]].indexVn = 0;
				count[3]++;
			}
		}
	/*	for (int j = 0; j < count[3]; j++)
			printf("%d %d %d\n", faces[j].indexV, faces[j].indexVt, faces[j].indexVn);*/

		fclose(model);
}

void draw_loaded_obj()

{

	static int noFile = 1;
	if (faces != NULL) {
		int i = 0;
		while (1)
		{
			glBindTexture(GL_TEXTURE_2D, texture);
			glBegin(GL_POLYGON);
				while (faces[i].indexV != 0) 
				{
					glTexCoord2f(vt[faces[i].indexVt].x, vt[faces[i].indexVt].y);
					glNormal3f(vn[faces[i].indexVn].x, vn[faces[i].indexVn].y, vn[faces[i].indexVn].z);
					glVertex3f(v[faces[i].indexV].x, v[faces[i].indexV].y, v[faces[i].indexV].z);
					i++;
				}
			glEnd();
			glBindTexture(GL_TEXTURE_2D, 0);
			i++;
			if (i == count[3])
				break;
		}
	}
	else if (noFile==1)
	{
		printf("no obj file to be loaded in");
		noFile = 0;
	}


}

