#ifndef COLLISION_H
#define COLLISION_H
#include "vecops.h"

extern int collision(Vec3 camPosNextFrame);
extern void collision_on_obj_mov();
extern int get_unstuck();

#endif