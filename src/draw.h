#ifndef DRAW_H
#define DRAW_H

#include "loader.h"

//struktura za cuvanje podataka o povezanosti verteksa, koordinatama tekstura, koordinatama normala dobijenih pljosni (povezivanjem verteksa)
typedef struct {
	int indexV;
	int indexVt;
	int indexVn;
}Faces;


//struktura za cuvanje podataka potrebnih za iscrtavanje i koliziju objekata ucitanih iz .obj fajlova
typedef struct{
	Vec3 *v; 	/*Obj file info https://www.fileformat.info/format/wavefrontobj/egff.htm */
	Vec2 *vt;
	Vec3 *vn;
	Faces *faces;

	int count[4]; // podatak o broju v vt vn f
	
	char* name;

	Vec3 pos;

	//podaci potrebni za koliziju i bounding box
	float maxX;
	float maxY;
	float maxZ;
	float minX;
	float minY;
	float minZ;

	float centerX;
	float centerY;
	float centerZ;

	float width;
	float height;
	float depth;
}Object;



extern int objNum;
extern Object obj[];

extern void draw_loaded_obj();

extern void draw_skybox();

extern void update_obj_min_max(Object * obj, float comparisonX, float comparisonY, float comparisonZ);

extern void set_starting_bounding_box(Object * obj);

extern void update_obj_dimensions(Object * obj);

extern void draw_bounding_box(Object obj);

#endif