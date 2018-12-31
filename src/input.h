#ifndef INPUT_H
#define INPUT_H

#include <GL/glut.h>

extern float xCenter, yCenter;
extern GLfloat pitch, yaw, roll;

extern void on_keyboard(unsigned char key, int x, int y);

extern void on_mouse_mov( int xPos, int yPos);

extern void on_special_keyboard_in(int key, int x, int y);


#endif
