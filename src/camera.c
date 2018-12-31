
#include <math.h>
#include <stdio.h>

#include "camera.h"


Vec3 normCamDir = { 0.0f };
Vec3 viewPoint = { 0.0f };
Camera cam = { 5.0f,5.0f,5.0f,
			 0.0f,0.0f,1.0f,
			 0.0f,1.0f,0.0f,
			 1.0f,0.0f,0.0f};




void adjust_camera() {

	//na poziciju kamere se dodaje vektor pravca u koji se gleda, inace bi stalno gledali u prostor koji se nalazi izmedju vrednosti  0-1 koord pocetka

	viewPoint = add_vec(cam.pos,normCamDir);

	gluLookAt(cam.pos.x, cam.pos.y, cam.pos.z, 
			viewPoint.x,viewPoint.y,viewPoint.z, 
			cam.upVec.x, cam.upVec.y, cam.upVec.z);
}

void rot_cam(GLfloat pitch, GLfloat yaw)
{
	cam.dir.x = (float)cos(PI / 180 * yaw) * (float)cos(PI / 180 * pitch);
	cam.dir.y = (float)sin(PI / 180 * pitch);
	cam.dir.z = (float)sin(PI / 180 * yaw) * (float)cos(PI / 180 * pitch);

	normCamDir = normalize_vec3(cam.dir);
}

void rot_x(GLfloat angle)
{
//	printf("%f %f %f \n", normCamDir.x, normCamDir.y, normCamDir.z);
	cam.dir.x = cam.dir.x*(float)cos(PI / 180 * angle) -cam.dir.z* (float)sin(PI/180 * angle);
	cam.dir.z = cam.dir.x*(float)sin(PI / 180 * angle) +cam.dir.z*(float)cos(PI/180 * angle);

	normCamDir = normalize_vec3(cam.dir);
//	printf("%f %f %f \n", normCamDir.x, normCamDir.y, normCamDir.z);
}



//pomeranja u prostoru ostvarena promenom x,y,z vrednosti eye argumenata funkcije glUlookAt
void mov_forw(GLfloat distance)
{
	cam.pos = add_vec(cam.pos, mul_vec_scalar(cam.dir, -distance));
}

//da se ne bismo stalno kretali po x osi koordinatnog pocetka potrebno je da uzmemo u obzir gde trenutno gledamo i nadjemo upravan vektor na taj vector (camera direction) i up vector
void mov_right(GLfloat distance)
{
	Vec3 res=normalize_vec3(crossproduct(cam.dir, cam.upVec));
	cam.pos = add_vec(cam.pos, mul_vec_scalar(res, distance));
}
//gore navedenu stvar ne cinimo za kretanje ka gore jer ne zelimo da smer kamere utice na to gde cemo se kretati
void mov_up(GLfloat distance)
{
	cam.pos = add_vec(cam.pos, mul_vec_scalar(cam.upVec, distance));
}
