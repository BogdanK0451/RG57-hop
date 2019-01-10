#include <math.h>

#include <stdio.h>
#include "collision.h"
#include "camera.h"
#include "draw.h"
#include "input.h"


int collision(Vec3 camPosNextFrame)
{

	for (int i = 0; i < objNum; i++)
	{
		//printf("%f %f %f\n %f %f %f\n", cam.pos.x, cam.pos.y, cam.pos.z, camPosNextFrame.x, camPosNextFrame.y, camPosNextFrame.z);
			//check the X axis								//dodajemo kameru, da ne bih gledali kroz objekat ukoliko smo previse blizu
		if (fabs(obj[i].centerX - camPosNextFrame.x) < obj[i].width/2 + cam.width)
		{
			//check the Y axis
			if (fabs(obj[i].centerY - camPosNextFrame.y) < obj[i].height/2 + cam.height)
			{
				//check the Z axis
				if (fabs(obj[i].centerZ - camPosNextFrame.z) < obj[i].depth/2 + cam.depth)
				{
					return 1;
				}
			}
		}
	}
	return 0;
}
