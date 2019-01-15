#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glut.h>

#include "vecops.h"

#define PI 3.1415926

//struktura sa podacima o igracu
typedef struct{
	Vec3 pos;
	Vec3 dir;
	Vec3 upVec;
	Vec3 rVec;
	float width;
	float height;
	float depth;
	float speedMultiplier;
	float jumpVelocity;
	int onGround;
	int crouched;
	int spellCooldown;
	int powerCharge;
	int health;
	float fov;
}Camera;
//struktura za podatke o gravitaciji
typedef struct{
	float velocity;
	float accel;
	float maxVelocity;
}Gravity;

extern Vec3 viewPoint;
extern Camera cam;
extern Vec3 normCamDir;
extern Gravity grav;

extern GLfloat pitch;
extern GLfloat yaw;
extern GLfloat roll;	//placeholder

extern void adjust_cam();
extern void cam_control(GLfloat pitch, GLfloat yaw);
extern void mov_forw(GLfloat distance);
extern void mov_right(GLfloat distance);
extern void mov_up(GLfloat distance);
extern void crouch(int mode);
extern void jump();
extern void blink_forw(float distance,float decelleration, int mode);
extern void blink_right(float distance, float decelleration, int mode);
extern void teleport();
extern void jetpack(float distance);
extern void gravity();
extern void check_if_pitch_89();
extern void check_fov_val();
extern void death_cam();

#endif