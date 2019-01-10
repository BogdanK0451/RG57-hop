
#include <math.h>
#include <stdio.h>

#include "collision.h"
#include "camera.h"


Vec3 normCamDir = { 0.0f };
Vec3 viewPoint = { 0.0f };
Camera cam = { 5.0f,5.0f,5.0f,  // pozicija kamere
			 0.0f,0.0f,1.0f,	// vektor pravca u koji se gleda
			 0.0f,1.0f,0.0f,	// upvector
			 1.0f,0.0f,0.0f,	// rightvector
			 1.5f,3.0f,1.5f};   //visina,sirina,dubina igraca/kamere




void adjust_camera() {
	//na poziciju kamere se dodaje vektor pravca u koji se gleda, inace bi stalno gledali u prostor koji se nalazi izmedju vrednosti  0-1 koord pocetka
	viewPoint = add_vec(cam.pos,normCamDir);

	gluLookAt(cam.pos.x, cam.pos.y, cam.pos.z, 
			viewPoint.x,viewPoint.y,viewPoint.z, 
			cam.upVec.x, cam.upVec.y, cam.upVec.z);
}

void rot_cam(GLfloat pitch, GLfloat yaw)
{
	cam.dir.x = (float)cos(PI / 180 * yaw) * (float)cos(PI / 180 * -pitch);
	cam.dir.y = (float)sin(PI / 180 * -pitch);
	cam.dir.z = (float)sin(PI / 180 * yaw) * (float)cos(PI / 180 * -pitch);

	normCamDir = normalize_vec3(cam.dir);
}

//pomeranja u prostoru ostvarena promenom x,y,z vrednosti eye argumenata funkcije glUlookAt
void mov_forw(GLfloat distance)
{
	if (collision(add_vec(cam.pos, mul_vec_scalar(cam.dir, distance))))
		return;
	cam.pos = add_vec(cam.pos, mul_vec_scalar(cam.dir, distance));
}


void mov_right(GLfloat distance)
{	//da se ne bismo stalno kretali paralelno x osi potrebno je da uzmemo u obzir gde trenutno gledamo i nadjemo upravan vektor na taj vector (camera direction) i up vector
	Vec3 res = normalize_vec3(crossproduct(cam.dir, cam.upVec));


	if (collision(add_vec(cam.pos, mul_vec_scalar(res, distance))))
		return;

	cam.pos = add_vec(cam.pos, mul_vec_scalar(res, distance));
}
//gore navedenu stvar ne cinimo za kretanje ka gore jer ne zelimo da smer kamere utice na to gde cemo se kretati
void mov_up(GLfloat distance)
{
	if (collision(add_vec(cam.pos, mul_vec_scalar(cam.upVec, distance))))
		return;
	cam.pos = add_vec(cam.pos, mul_vec_scalar(cam.upVec, distance));
}
