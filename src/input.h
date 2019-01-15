#ifndef INPUT_H
#define INPUT_H

#include <GL/glut.h>

#define WINDOW_WIDTH 1650
#define WINDOW_HEIGHT 1050

#define ESC 27
#define SPACE 32
#define TAB 9
#define MOVEMENT_SPEED 0.2f
#define BLINK_DISTANCE 1.1f
#define TELEPORT_DISTANCE_SEGMENTS 0.5f
#define DECELLERATION -0.01f

extern float xCenter, yCenter;

extern int KEY_TAB;
extern int MULTIPLE_VIEWPORTS;
extern int TEXTURELESS_MODE;
extern int DAY;
extern int NO_CLIP;
extern int SHOW_BOUNDARIES;
extern int RENDER_MODE;

extern void mov_control();
extern void on_keyboard(unsigned char key, int x, int y);
extern void on_keyboard_up(unsigned char key, int x, int y);
extern void on_mouse_mov( int xPos, int yPos);
extern void on_mouse_mov_and_click(int xPos, int yPos);
extern void on_mouse_click(int button, int state, int x, int y);
extern void on_special_keyboard(int key, int x, int y);
extern void on_special_keyboard_up(int key, int x, int y);

#endif