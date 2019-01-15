#include <math.h>

#include <stdio.h>
#include "collision.h"
#include "camera.h"
#include "draw.h"
#include "input.h"

/*proveravamo koliziju izmedju igraca i objekata, provera se vrsi samo kad se igrac krece
u slucaju da kretanje dovodi do kolizije u sledecem frejmu, kretanje se ne izvrsava*/
int collision(Vec3 camPosNextFrame) 
{
	if (!NO_CLIP)
	{
		for (int i = 0; i < objNum; i++)
		{
			//X osa kolizija							//dodajemo kameru, da bih drzali odstojanje od objekata pri koliziji (da kamera ne bih "prolazila kroz njih")
			if (fabs(obj[i].centerX - camPosNextFrame.x) < obj[i].width / 2 + cam.width)
			{
				//Y osa kolizija
				if (fabs(obj[i].centerY - camPosNextFrame.y) < obj[i].height / 2 + cam.height)
				{
					//Z kolizija
					if (fabs(obj[i].centerZ - camPosNextFrame.z) < obj[i].depth / 2 + cam.depth)
					{
						if (obj[i].deadly)
							cam.health = 0;
						obj[i].collided = 1;
						cam.onGround = 1;	// u slucaju kolizije smatramo da je igrac na zemlji
						return 1;
					}
				}
			}
			obj[i].collided = 0;
		}
		return 0;
	}
	return 0;
}
/*provera kolizije izmedju igraca i objekata, za razliku od collision() 
ova funkcija se poziva kada se objekat krece, a ne igrac krece*/
void collision_on_obj_mov()
{
	if (!NO_CLIP)
	{
		for (int i = 0; i < objNum; i++)
		{
			if (obj[i].deadly) //posto se u igri krecu samo objekti koji mogu da ubiju igraca, nastavljamo sa proverom samo za njih
			{				
				if (fabs(obj[i].centerX-cam.pos.x) < obj[i].width / 2 + cam.width)
				{
					if (fabs(obj[i].centerY - cam.pos.y) < obj[i].height / 2 + cam.height)
					{
						if (fabs(obj[i].centerZ - cam.pos.z) < obj[i].depth / 2 + cam.depth)
						{
							cam.health = 0;
							obj[i].collided = 1;
						}
					}
				}
			}
		}
	}
}
/*poziva se pri prelasku iz NO_CLIP moda u obican mod da bi se igrac odglavio u slucaju da je 
tokom NO_CLIP moda zavrsio u zidu*/
int get_unstuck()
{
	for (int i = 0; i < objNum; i++)
	{
		if (fabs(obj[i].centerX - cam.pos.x) < obj[i].width / 2 + cam.width)
		{
			if (fabs(obj[i].centerY - cam.pos.y) < obj[i].height / 2 + cam.height)
			{
				if (fabs(obj[i].centerZ - cam.pos.z) < obj[i].depth / 2 + cam.depth)
				{
					cam.pos.y +=  1;	// igrac "isplivava na gore" iz rekurzije izlazimo kada nema kolizije
					get_unstuck();		
				}
			}
		}
	}
	return 1;
}