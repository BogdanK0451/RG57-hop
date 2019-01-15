#ifndef LOADER_H 
#define LOADER_H
#include "vecops.h"

typedef struct {
	GLuint id;
	char name[40];
}Texture;

typedef struct {
	char *modelPath;
	int modelCount;
}ModelPaths;

extern ModelPaths modelPaths[];
extern char *skyboxPaths[];

extern Texture tex[];
extern GLuint skybox[];
extern int texNum;

extern void load_skybox(char **path);
extern void load_models(ModelPaths *modelPaths,char *mapPath);
extern void load_texture(char *path);
extern void load_all_files();

#endif