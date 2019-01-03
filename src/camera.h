#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glut.h>

#include "vecops.h"


#define PI 3.1415926


typedef struct {
	Vec3 pos;
	Vec3 dir;
	Vec3 upVec;
	Vec3 rVec;
}Camera;

extern Vec3 viewPoint;
extern Camera cam;
extern Vec3 normCamDir;


extern void adjust_camera();

extern void rot_cam(GLfloat pitch, GLfloat yaw);

extern void mov_forw(GLfloat distance);

extern void mov_right(GLfloat distance);

extern void mov_up(GLfloat distance);



#endif


