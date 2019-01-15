#ifndef DRAW_H
#define DRAW_H

#include "loader.h"

#define MAX_LIGHTS 2
#define NUM_OF_OBJ 600	//izdvaja se memorija za strukturu od 600 clanova	
#define OBJ_TO_USE_EFFECT1_ON "stormtrooper"
#define OBJ2_TO_USE_EFFECT1_ON "glock"

//struktura za cuvanje podataka o povezanosti verteksa, koordinata tekstura, koordinata normala
typedef struct{
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

	float maxX;		//podaci potrebni za koliziju i bounding box
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
	
	int collided;
	int deadly;
}Object;

extern Object obj[];
//brzina efekta koja se koristi u funkciji polygon_draw_speed
extern int effect1Speed;
extern int lights[];
extern int objNum;

extern void draw_loaded_obj(float angle,int tick);
extern int polygon_draw_limit(int j);
extern Vec3 animate_cube(int j);
extern void draw_with_texture(int i, int j);
extern void draw_without_texture(int i, int j);
extern void draw_bounding_box(Object obj,Vec3 mov);
extern void draw_skybox();
extern void draw_ui(int windowHeight, int windowWidth);
extern void draw_coordinate_system();
extern void draw_light(float angle);
extern void draw_mode();
extern void update_obj_min_max(Object * obj, float comparisonX, float comparisonY, float comparisonZ);
extern void set_starting_bounding_box(Object * obj);
extern void update_obj_dimensions(Object * obj);
extern void update_obj_center(Vec3 translate, int j);

#endif