#ifndef LOADER_H 
#define LOADER_H

#include "vecops.h"


typedef struct {
	int indexV;
	int indexVt;
	int indexVn;
}Faces;


extern Vec3 *v;		/*Obj file info https://www.fileformat.info/format/wavefrontobj/egff.htm */
extern Vec2 *vt;
extern Vec3 *vn;
extern Faces *faces;

extern void load_model(char* path);

extern void draw_loaded_obj();


#endif