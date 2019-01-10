#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>

#include "loader.h"
#include "../external/re.h"
#include "draw.h"
#include "vecops.h"
#include "float.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"

#define NUM_OF_OBJ 200

#define STRUCT_SIZE sizeof(modelPaths)/sizeof(modelPaths[0])

/* funkcije u ovom fajlu sluze za ucitavanje podataka o objektima tipa .obj i teksturama tipa .bmp .jpg itd

	vise o ovom formatu se moze videti ->  Obj file info https://www.fileformat.info/format/wavefrontobj/egff.htm 

	podrzani su tokeni "tipa" 
	verteksi					v x y z          
	normale verteksa			vn x y z
	vt koordinate za teksture	vt x y
	pljosni/faces				f  x y z  ili f x/x y/y z/z ili f x//x y//y z//z ili f x/y/z

	unos fajla koji ne postuje ovaj format (neki fajlovi na internetu ne postoju ovu konkretnu notaciju i pri ucitavanju takvih ponasanje je nepoznato
	

*/


Object obj[NUM_OF_OBJ];
int objNum = 0;

Texture tex[NUM_OF_OBJ];
int texNum = 0;

GLuint skybox[6];

char *skyboxPaths[] = { "resources/textures/skybox/cloudtop_ft.tga",
						"resources/textures/skybox/cloudtop_lf.tga",
						"resources/textures/skybox/cloudtop_bk.tga",
						"resources/textures/skybox/cloudtop_rt.tga",
						"resources/textures/skybox/cloudtop_up.tga",
						"resources/textures/skybox/cloudtop_dn.tga"
					};

ModelPaths modelPaths[] = { "resources/models/grass.obj",50,
			    "resources/models/stormtrooper.obj",0
			};


void load_all_files()
{
	load_skybox(skyboxPaths);
	
	load_texture("resources/textures/stormtrooper.tga");		//ime teksture mora da se poklapa sa imenom modela i ne sme sadrzati karaktere / .
	load_texture("resources/textures/grass.png");				//ukoliko ima vise imena tekstura koja se sadrze u imenu objekta, uzimamo u obzir prvi ucitan a ostale ignorisemo


	load_models(modelPaths, "resources/maps/firstmap.txt");
}



void load_texture(char *path)
{

	int components;
	int tWidth, tHeight;
	//ucitavamo teksturu i okrecemo je vertikalno da bi bila usaglasena sa "standardom" Opengl
	stbi_set_flip_vertically_on_load(1);
	unsigned char* data = stbi_load(path, &tWidth, &tHeight, &components, 3);
	
	if(data==NULL)
		fprintf(stderr, " Error opening texture: %s\n Object will be textureless", strerror(errno));

	char * token;
	char buf[128];
	strcpy(buf,path);

	token = strtok(buf, "/.");
	token = strtok(NULL, "/.");
	token = strtok(NULL, "/."); //izdvajamo ime objeta iz resources/tex/.. .tga
		
	strcpy(tex[texNum].name,token);

	glGenTextures(1, &tex[texNum].id);
	glBindTexture(GL_TEXTURE_2D, tex[texNum].id);
	// Parametri teksture
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	texNum++;
}

void load_skybox(char **path)
{
	int components;
	int tWidth, tHeight;

	//moguce je i implementirati pomocu GL_TEXTURE_CUBE_MAP i onda umesto 6 2D tekstura bi smo imali jednu
	stbi_set_flip_vertically_on_load(1);
	glGenTextures(6, skybox);

	unsigned char *data;

	for (int i = 0; i < 6; i++)
	{
		glBindTexture(GL_TEXTURE_2D, skybox[i]);
		data = stbi_load(path[i], &tWidth, &tHeight, &components, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D,0, GL_RGB, tWidth, tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			if (data == NULL)
				fprintf(stderr, " Error opening texture: %s\n Skybox will be textureless", strerror(errno));
			stbi_image_free(data);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void load_models(ModelPaths *paths ,char *mapPath)
{
	FILE *map = fopen(mapPath, "r");

	if (map == NULL)
	{
		fprintf(stderr, " Error opening .map file: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	int structSize = STRUCT_SIZE;

	for (int i = 0; i < structSize; i++)
	{

		for (int c = 0; c < paths[i].modelCount; c++)
		{
			FILE *model = fopen(paths[i].modelPath, "r");
			if (model == NULL)
			{
				fprintf(stderr, " Error opening .obj file: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

			obj[objNum].name = modelPaths[i].modelPath;

			obj[objNum].count[0] = 1; // - index vertex-a						Prva tri indeksa su postavljena na 1 jer pri crtanju objekta moramo da povezemo vertexe zadate sa face-ovima
			obj[objNum].count[1] = 1; // - index koordinata						npr. f 1 2 5 7 a neki od faceova ukljucuju vertex 1 pa ukoliko bismo kretali od 0 uvek bi se taj vertex preskocio          
			obj[objNum].count[2] = 1; // - index normala						i poligon bi pogresno bio iscrtan	
			obj[objNum].count[3] = 0; // - index pljosni

			// pocetne velicine nizova
			int vSize = 20;
			int vtSize = 20;
			int vnSize = 20;
			int facesSize = 20;

			//dinamicka alokacija
			obj[objNum].v = (Vec3*)malloc(sizeof(Vec3) * vSize);
			obj[objNum].vt = (Vec2*)malloc(sizeof(Vec2) * vtSize);
			obj[objNum].vn = (Vec3*)malloc(sizeof(Vec3) * vnSize);
			obj[objNum].faces = (Faces*)malloc(sizeof(Faces) * facesSize);


			//provera gresaka za alokaciju
			if (!obj[objNum].v || !obj[objNum].vt || !obj[objNum].vn || !obj[objNum].faces)
			{
				fprintf(stderr, " Error allocating memory: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

			// postavljaju se pocetne koordinate kocke koja sluzi za koliziju, na FLOAT_MIN FLOAT_MAX
			set_starting_bounding_box(&obj[objNum]);

			//bafer za koordinate polozaja objekta iz map fajla
			char posBuf[256];
			if (fgets(posBuf, 256, map) == NULL)
				strcpy(posBuf, "0.0 0.0 0.0");			//u slucaju da nema vise koordinata, postavi objekat na x=0 y=0 z=0

			sscanf(posBuf, "%*s %f %f %f", &obj[objNum].pos.x, &obj[objNum].pos.y, &obj[objNum].pos.z);

			while (1)
			{
				char buf[256]; //bafer za podatke o .obj fajlu
				if (fgets(buf, 256, model) == NULL)
					break;

				//parsiramo vertekse

				if (re_match("^v[^tn]", buf) != -1)
				{
					if (obj[objNum].count[0] >= vSize/2)
					{
						vSize = vSize * 2;
						//newptr= realloc(oldptr,size)
						obj[objNum].v = (Vec3*)realloc(obj[objNum].v, sizeof(Vec3) * vSize);
					}
					sscanf(buf, "%*s %f %f %f", &obj[objNum].v[obj[objNum].count[0]].x, &obj[objNum].v[obj[objNum].count[0]].y, &obj[objNum].v[obj[objNum].count[0]].z);

					//postavljamo min/max koordinate potrebne za iscrtavanje bounding kocke
					update_obj_min_max(&obj[objNum],
						obj[objNum].v[obj[objNum].count[0]].x,
						obj[objNum].v[obj[objNum].count[0]].y,
						obj[objNum].v[obj[objNum].count[0]].z);

					obj[objNum].count[0]++;
				}

				//parsiramo koordinate tekstura

				else if (re_match("^vt", buf) != -1)
				{
					if (obj[objNum].count[1] == vtSize/2)
					{
						vtSize = vtSize * 2;
						obj[objNum].vt = (Vec2*)realloc(obj[objNum].vt, sizeof(Vec2) * vtSize);
					}
					sscanf(buf, "%*s %f %f", &obj[objNum].vt[obj[objNum].count[1]].x, &obj[objNum].vt[obj[objNum].count[1]].y);
					obj[objNum].count[1]++;
				}

				//parsiramo normale

				else if (re_match("$vn", buf) != -1)
				{
					if (obj[objNum].count[2] == vnSize/2)
					{
						vnSize = vnSize * 2;
						//newptr= realloc(oldptr,size)
						obj[objNum].vn = (Vec3*)realloc(obj[objNum].vn, sizeof(Vec3) * vnSize);
					}
					sscanf(buf, "%*s %f %f %f", &obj[objNum].vn[obj[objNum].count[2]].x, &obj[objNum].vn[obj[objNum].count[2]].y, &obj[objNum].vn[obj[objNum].count[2]].z);
					obj[objNum].count[2]++;
				}

				//parsiramo faces/pljosni

				else if (buf[0] == 'f' && (buf[1] == ' '))
				{
					char * token;

					if (obj[objNum].count[3] >= facesSize/2)
					{
						facesSize = facesSize * 2;
						obj[objNum].faces = (Faces*)realloc(obj[objNum].faces, sizeof(Faces) * facesSize);
					}

					if (re_match("/", buf) != -1)
					{

						if (re_match("[0-9]*/[0-9]*/[0-9]* ", buf) != -1)   //  f  x/x/x y/y/y /z/z/z
						{
							int j = 1;
							token = strtok(buf, "/ ");
							while (token != NULL)
							{
								if (strcmp(token, "f"))
								{
									if (j % 3 == 1)
									{
										obj[objNum].faces[obj[objNum].count[3]].indexV = atoi(token);
									}

									else if (j % 3 == 2)
									{
										obj[objNum].faces[obj[objNum].count[3]].indexVt = atoi(token);
									}
									else if (j % 3 == 0)
									{
										obj[objNum].faces[obj[objNum].count[3]].indexVn = atoi(token);
										obj[objNum].count[3]++;
									}
									j++;
								}
								token = strtok(NULL, "/ ");
							}
						}

						else if (re_match("[0-9]*/[0-9]*", buf) != -1)   //  f  x/x y/y /z/z
						{
							int j = 1;
							token = strtok(buf, "/ ");
							while (token != NULL)
							{
								//ignorisemo token  f\0, interesuju nas samo numericke vrednosti
								if (strcmp(token, "f"))
								{
									if (j % 2 == 1)
									{
										obj[objNum].faces[obj[objNum].count[3]].indexV = atoi(token);

									}

									else if (j % 2 == 0)
									{
										obj[objNum].faces[obj[objNum].count[3]].indexVt = atoi(token);
										obj[objNum].faces[obj[objNum].count[3]].indexVn = 0;
										obj[objNum].count[3]++;
									}
									j++;
								}
								token = strtok(NULL, "/ ");
							}
						}

						else if (re_match("//", buf) != -1)   //  f  x//x y//y /z//z
						{
							int j = 1;
							token = strtok(buf, "/ ");
							while (token != NULL)
							{
								if (strcmp(token, "f"))
								{
									if (j % 2 == 1)
									{
										obj[objNum].faces[obj[objNum].count[3]].indexV = atoi(token);
									}

									else if (j % 2 == 0)
									{
										obj[objNum].faces[obj[objNum].count[3]].indexVn = atoi(token);
										obj[objNum].faces[obj[objNum].count[3]].indexVt = 0;
										obj[objNum].count[3]++;
									}
									j++;
								}
								token = strtok(NULL, "/ ");
							}
						}
					}
					else	// f x y z
					{
						token = strtok(buf, " ");
						while (token != NULL)
						{
							if (strcmp(token, "f"))
							{
								obj[objNum].faces[obj[objNum].count[3]].indexV = atoi(token);
								obj[objNum].faces[obj[objNum].count[3]].indexVt = 0;
								obj[objNum].faces[obj[objNum].count[3]].indexVn = 0;
								obj[objNum].count[3]++;
							}
							token = strtok(NULL, " ");
						}
					}
					//razdvajanje dve pljosni u nizu nulama
					obj[objNum].faces[obj[objNum].count[3]].indexV = 0;
					obj[objNum].faces[obj[objNum].count[3]].indexVt = 0;
					obj[objNum].faces[obj[objNum].count[3]].indexVn = 0;
					obj[objNum].count[3]++;
				}
			}

			obj[objNum].minX += obj[objNum].pos.x;
			obj[objNum].maxX += obj[objNum].pos.x;
			obj[objNum].minY += obj[objNum].pos.y;
			obj[objNum].maxY += obj[objNum].pos.y;
			obj[objNum].minZ += obj[objNum].pos.z;
			obj[objNum].maxZ += obj[objNum].pos.z;

			update_obj_dimensions(&obj[objNum]);
			fclose(model);

			objNum++;
		}


	}
	fclose(map);
}
