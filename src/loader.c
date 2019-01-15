#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>

#include "loader.h"
#include "draw.h"
#include "vecops.h"
#include "float.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"	//biblioteka za ucitavanje slika
//#include "../external/glext.h"		//biblioteka bez koje nije moguce (na windowsu) koristiti GL_CLAMP_TO_EDGE
#include "../external/re.h"		//biblioteka za regexp


#define STRUCT_SIZE sizeof(modelPaths)/sizeof(modelPaths[0])

/* funkcije u ovom fajlu sluze za ucitavanje podataka o objektima tipa .obj i teksturama tipa .bmp .jpg .png itd
	
	Ucitavanje objekata se izvodi parsiranjem .obj fajla i cuvanjem podataka u strukturi Object

	Vise o ovom formatu se moze videti na linku -> TL:DR https://www.cs.cmu.edu/~mbz/personal/graphics/obj.html
	
	za one koji zele detaljnije obrazlozenje ->	http://www.martinreddy.net/gfx/3d/OBJ.spec

	Kao sto se na linku moze videti, ovaj format sadrzi mnostvo drugih tokena
	ali osim dole navedenih oni trenutno nisu podrzani jer nisu potrebni za "jednostavno" iscrtavanje scene
	(npr ako se ucitava .obj koji u sebi sadrzi mnostvo podobjekata ovaj parser ce ih ucitati kao jednu celinu
	sto onemogucava manipulisanje svakim od podobjekata ponaosobom)

	podrzani su tokeni tipa:
	
	vertex	v						v  x y z          
	koordinate za teksture vt 		vt u v 
	normala verteksa	vn			vn i j k
	faces/pljosni 
	(face se moze sastojati iz		f v v v 
	proizvoljnog br. verteksa <2)	f v/vt v/vt v/vt ili 
									f v//vn v//vn v//vn ili 
									f v/vt/vn v/vt/vn v/vt/vn

	Pri unosu fajlova koji ne postuju ovaj format (neki fajlovi na internetu ne koriste ovu konkretnu notaciju) ponasanje je nedefinisano i moguce je da ce program puci)
*/


Object obj[NUM_OF_OBJ];
int objNum = 0;

Texture tex[NUM_OF_OBJ];
int texNum = 0;

GLuint skybox[6];
/*slike za teksture za skybox*/
char *skyboxPaths[] = { "resources/textures/skybox/cloudtop_ft.tga",	
						"resources/textures/skybox/cloudtop_lf.tga",
						"resources/textures/skybox/cloudtop_bk.tga",
						"resources/textures/skybox/cloudtop_rt.tga",
						"resources/textures/skybox/cloudtop_up.tga",
						"resources/textures/skybox/cloudtop_dn.tga"
					};
/*format->  {"putanja do modela",broj kopija modela}*/
ModelPaths modelPaths[] = { {"resources/models/grass.obj",48},			
			    {"resources/models/snow.obj",172},
			    {"resources/models/stormtrooper.obj",1},
			    {"resources/models/glock.obj",1},
			    {"resources/models/smallcube.obj",28},
			    {"resources/models/bigcube1.obj",51},
			    {"resources/models/mediumcube1.obj",16},
			    {"resources/models/biggestcube1.obj",58},
			    {"resources/models/bigcube2.obj",44},
			    {"resources/models/deadlycube.obj",6}
							};
/*ime teksture mora da se poklapa sa imenom modela i ne sme sadrzati karaktere / . 
ukoliko postoji vise imena tekstura koja se sadrze u imenu objekta (teksture se vezuju za objekte ako strstr(objekat,tekstura)!=0), uzimamo u obzir prvu ucitanu a ostale ignorisemo*/
void load_all_files()
{
		load_skybox(skyboxPaths);

		load_texture("resources/textures/stormtrooper.tga");
		load_texture("resources/textures/grass.png");	
		load_texture("resources/textures/snow.png");
		load_texture("resources/textures/glock.png");
		load_texture("resources/textures/map.jpg");
		load_texture("resources/textures/smallcube.png");
		load_texture("resources/textures/mediumcube1.png");
		load_texture("resources/textures/bigcube1.png");
		load_texture("resources/textures/biggestcube1.png");
		load_texture("resources/textures/bigcube2.png");
		load_texture("resources/textures/deadlycube.png");

		load_models(modelPaths, "resources/maps/firstmap.txt");
}
/*ucitavamo sliku koristeci stb_image.h i vezujemo je za generisanu teksturu*/
void load_texture(char *path)
{
	int components;
	int tWidth, tHeight;
	/*ucitavamo teksturu i okrecemo je vertikalno da bi bila usaglasena sa "standardom" Opengl*/
	stbi_set_flip_vertically_on_load(1);
	unsigned char* data = stbi_load(path, &tWidth, &tHeight, &components, 3);
	
	if(data==NULL)
		fprintf(stderr, " Error opening texture: %s\n Object will be textureless", strerror(errno));

	char *token;
	char buf[128];
	strcpy(buf, path);

	token = strtok(buf, "/.");
	token = strtok(NULL, "/.");
	token = strtok(NULL, "/."); //izdvajamo ime objeta iz resources/tex/ime.tga

	strcpy(tex[texNum].name, token);

	glGenTextures(1, &tex[texNum].id);
	glBindTexture(GL_TEXTURE_2D, tex[texNum].id);
	/*Parametri teksture*/
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //biramo da li osvetljenje deluje na teksturu ili ne GL_REPLACE GL_MODULATE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	texNum++;
}
/*skybox se ucitava tako sto se ucita 6 slika koje se vezuju za 6 tekstura
moguce je i implementirati pomocu GL_TEXTURE_CUBE_MAP umesto GL_TEXTURE_2D i onda bismo umesto 6 2D tekstura imali jednu*/
void load_skybox(char **path)
{
	int components;
	int tWidth, tHeight;

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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //GL_CLAMP_EDGE na windows-uu ne radi bez ukljucivanja glext.h koje zove khrplatform.h
	}																		//za one koji ovo hoce da pokrenu na windowsu, ovaj projekat sadrzi navedena zaglavlja
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}
/* model se ucitava parsiranjem podataka iz .obj fajla i dodeljivanjem pozicije datom objektu
koristeci koordinate iz mape*/
void load_models(ModelPaths *paths ,char *mapPath)
{
	FILE *map = fopen(mapPath, "r");	//fajl sa koordinatama pozicije objekata na sceni

	if (map == NULL)
	{
		fprintf(stderr, "Error opening .map file: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	int structSize = STRUCT_SIZE;

	for (int i = 0; i < structSize; i++)
	{
		for (int c = 0; c < paths[i].modelCount; c++)
		{
			FILE *model = fopen(paths[i].modelPath, "r");	//obj. fajl sa podacima modela
			if (model == NULL)
			{
				fprintf(stderr, " Error opening .obj file: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

			obj[objNum].name = modelPaths[i].modelPath;
			if (strstr(modelPaths[i].modelPath, "deadly"))	//ako se ime poklapa objekat ce biti deadly, sto znaci da ce da ubija igraca pri dodiru
				obj[objNum].deadly = 1;

			obj[objNum].count[0] = 1; // - brojac/index vertex-a						Prva tri indeksa su postavljena na 1 jer pri crtanju objekta moramo da povezemo vertexe zadate sa face-ovima
			obj[objNum].count[1] = 1; // - brojac/index koordinata						npr. f 1 2 5 7 a neki od faceova ukljucuju vertex 1 pa ukoliko bismo kretali od 0 uvek bi se taj vertex preskocio          
			obj[objNum].count[2] = 1; // - brojac/index normala							i poligon bi pogresno bio iscrtan	
			obj[objNum].count[3] = 0; // - brojac/index pljosni

			/*pocetne velicine nizova*/
			int vSize = 20;
			int vtSize = 20;
			int vnSize = 20;
			int facesSize = 20;

			/*dinamicka alokacija za podatke unutar .obj fajla*/
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
			/* postavljaju se pocetne koordinate kocke koja sluzi za aproksimaciju kolizije, na FLOAT_MIN FLOAT_MAX*/
			set_starting_bounding_box(&obj[objNum]);
			/*bafer za koordinate na koje ce se postaviti objekti iz .obj fajla*/
			char posBuf[256];
			if (fgets(posBuf, 256, map) == NULL)
				strcpy(posBuf, "0.0 0.0 0.0");	//u slucaju da nema vise koordinata u mapi, postaviti objekat na default poziciju x=0 y=0 z=0

			sscanf(posBuf, "%f %f %f", &obj[objNum].pos.x, &obj[objNum].pos.y, &obj[objNum].pos.z);	//koordinate polozaja se objektima zadaju redom kojim su objekti ucitani

			while (1)
			{	/*/bafer za podatke o .obj fajlu*/
				char buf[256];
				if (fgets(buf, 256, model) == NULL)
					break;

				/*parsiranje verteksa*/
				if (re_match("^v[^tn]", buf) != -1)
				{
					if (obj[objNum].count[0] >= vSize/2)
					{
						vSize = vSize * 2;
						obj[objNum].v = (Vec3*)realloc(obj[objNum].v, sizeof(Vec3) * vSize);
					}
					sscanf(buf, "%*s %f %f %f", &obj[objNum].v[obj[objNum].count[0]].x, &obj[objNum].v[obj[objNum].count[0]].y, &obj[objNum].v[obj[objNum].count[0]].z);

					/*pri ucitavanju svakog novog verteksa obnavljamo min/max koordinate potrebne za iscrtavanje bounding kocke*/
					update_obj_min_max(&obj[objNum],
						obj[objNum].v[obj[objNum].count[0]].x,
						obj[objNum].v[obj[objNum].count[0]].y,
						obj[objNum].v[obj[objNum].count[0]].z);

					obj[objNum].count[0]++;
				}
				/*parsiranje koordinata tekstura*/
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
				/*parsiranje normala za osvetljenje*/
				else if (re_match("^vn", buf) != -1)
				{
					if (obj[objNum].count[2] == vnSize/2)
					{
						vnSize = vnSize * 2;
						obj[objNum].vn = (Vec3*)realloc(obj[objNum].vn, sizeof(Vec3) * vnSize);
					}
					sscanf(buf, "%*s %f %f %f", &obj[objNum].vn[obj[objNum].count[2]].x, &obj[objNum].vn[obj[objNum].count[2]].y, &obj[objNum].vn[obj[objNum].count[2]].z);
					obj[objNum].count[2]++;
				}
				/*parsiranje pljosni/faces*/
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
						if (re_match("[0-9]*/[0-9]*/[0-9]* ", buf) != -1)   // f  v/vt/vn v/vt/vn v/vt/vn
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

						else if (re_match("[0-9]*/[0-9]*", buf) != -1)   //  f v/vt v/vt v/vt
						{
							int j = 1;
							token = strtok(buf, "/ ");
							while (token != NULL)
							{
								//ignorisemo token  "f", interesuju nas samo numericke vrednosti
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

						else if (re_match("//", buf) != -1)   //  f v//vn v//vn v//vn
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

					else	// f v v v 
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
					/*razdvajamo dve pljosni u nizu nulama*/
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
			/*trazimo centar objekta i izracunavamo njegovu visinu sirinu duzinu*/
			update_obj_dimensions(&obj[objNum]);
			fclose(model);

			objNum++;
		}
	}
	fclose(map);
}
