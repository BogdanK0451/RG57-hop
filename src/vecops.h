#ifndef VECOPS_H
#define VECOPS_H

#include <GL/glut.h>


typedef struct {
	float x;
	float y;
	float z;
} Vec3;


extern GLfloat vector_len(Vec3 v);

extern Vec3 normalize_vec3(Vec3 v);

extern Vec3 crossproduct(Vec3 v1, Vec3 v2);

extern Vec3 add_vec(Vec3 v1, Vec3 v2);

extern Vec3 mul_vec(Vec3 v1, Vec3 v2);

extern Vec3 subs_vec(Vec3 v1, Vec3 v2);

extern Vec3 add_vec_scalar(Vec3 v1, float i);

extern Vec3 mul_vec_scalar(Vec3 v1, float i);

extern Vec3 subs_vec_scalar(Vec3 v1, float i);






#endif