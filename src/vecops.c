#include <GL/glut.h>

#include "vecops.h"
#include <math.h>

/*funkcije za vektorske operacije, mislim da nema potrebe sa dodatnim objasnjenjima*/

GLfloat vector_len(Vec3 v)
{
	return (GLfloat)(sqrt(v.x*v.x + v.y*v.y + v.z*v.z));
}

Vec3 normalize_vec3(Vec3 v)
{
	Vec3 res;
	float l = vector_len(v);
	res.x = v.x / l;
	res.y = v.y / l;
	res.z = v.z / l;
	return res;
}

Vec3 crossproduct(Vec3 v1, Vec3 v2)
{
	Vec3 resVector;
	resVector.x = v1.y*v2.z - v1.z*v2.y;
	resVector.y = v1.z*v2.x - v1.x*v2.z;
	resVector.z = v1.x*v2.y - v1.y*v2.x;

	return resVector;
}

Vec3 add_vec(Vec3 v1, Vec3 v2)
{
	Vec3 resVector;
	resVector.x = v1.x + v2.x;
	resVector.y = v1.y + v2.y;
	resVector.z = v1.z + v2.z;
	return resVector;
}

Vec3 mul_vec(Vec3 v1, Vec3 v2)
{
	Vec3 resVector;
	resVector.x = v1.x * v2.x;
	resVector.y = v1.y * v2.y;
	resVector.z = v1.z * v2.z;
	return resVector;
}

Vec3 subs_vec(Vec3 v1, Vec3 v2)
{
	Vec3 resVector;
	resVector.x = v1.x - v2.x;
	resVector.y = v1.y - v2.y;
	resVector.z = v1.z - v2.z;
	return resVector;
}

Vec3 add_vec_scalar(Vec3 v1, float i)
{
	Vec3 resVector;
	resVector.x = v1.x + i;
	resVector.y = v1.y + i;
	resVector.z = v1.z + i;
	return resVector;
}

Vec3 mul_vec_scalar(Vec3 v1, float i)
{
	Vec3 resVector;
	resVector.x = v1.x * i;
	resVector.y = v1.y * i;
	resVector.z = v1.z * i;
	return resVector;
}

Vec3 subs_vec_scalar(Vec3 v1, float i)
{
	Vec3 resVector;
	resVector.x = v1.x - i;
	resVector.y = v1.y - i;
	resVector.z = v1.z - i;
	return resVector;
}

