#ifndef INPUT_H
#define INPUT_H

#include <GL/glut.h>

extern float xCenter, yCenter;
extern GLfloat pitch, yaw, roll;
extern float gravityIntensity;
extern int SHOW_BOUNDARIES;

extern void on_keyboard(unsigned char key, int x, int y);

extern void on_keyboard_up(unsigned char key, int x, int y);

extern void call_movement_func();

extern void on_mouse_mov( int xPos, int yPos);

extern void on_special_keyboard_up(int key, int x, int y);

extern void on_special_keyboard(int key, int x, int y);

extern void check_if_pitch_89();


#endif