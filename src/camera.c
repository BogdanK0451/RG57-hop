#include <math.h>
#include <stdio.h>

#include "collision.h"
#include "camera.h"
#include "input.h"

Vec3 normCamDir = { 0.0f };
Vec3 viewPoint = { 0.0f };
Camera cam = {{7.0f,7.0f,7.0f},  // pozicija kamere
			  {0.0f,0.0f,1.0f},	// vektor pravca u koji se gleda
			  {0.0f,1.0f,0.0f},	// upvector
			  {1.0f,0.0f,0.0f},	// rightvector
			  0.5f,3.0f,0.5f,	 //visina,sirina,dubina igraca
			  1.0f,0.0f,0,		//multiplier brzine,brzina pri skoku, kamera na zemlji
			  0,0,180,			//crouch, cooldown, power charge
			  100,75			//health,fov
				};   

Gravity grav = {-0.0f,-0.015f,-2.0f }; // brzina,ubrzanje, maksimalna brzina   
									 
GLfloat pitch = 0;
GLfloat yaw = 0;
GLfloat roll = 0;	//placeholder

/*namestamo gluLookAt*/
void adjust_cam() 
{/*na poziciju kamere se dodaje normirani vektor pravca koji pretstavlja tacku u koju igrac gleda*/
	viewPoint = add_vec(cam.pos,normCamDir);

	gluLookAt(cam.pos.x, cam.pos.y, cam.pos.z, 
			viewPoint.x,viewPoint.y,viewPoint.z, 
			cam.upVec.x, cam.upVec.y, cam.upVec.z);
}
/*funkcija za upravljanje kamerom koja kao parametre prima vrednosti
koje predstavljaju pomeraj gore-dole i levo-desno i izracunava
normirani vektor pravca koji ce se iskoristiti pri izracunavanju nove
pozicije kamere u funkciji adjust_cam()*/
void cam_control(GLfloat pitch, GLfloat yaw)
{
	cam.dir.x = (float)cos(PI / 180 * yaw) * (float)cos(PI / 180 * -pitch);
	cam.dir.y = (float)sin(PI / 180 * -pitch);
	cam.dir.z = (float)sin(PI / 180 * yaw) * (float)cos(PI / 180 * -pitch);

	normCamDir = normalize_vec3(cam.dir);
}
/*promena polozaja u prostoru se ostvaruje promenom cam.pos. x,y,z koji se prosledjuje eye argumentu funkcije glUlookAt*/
void mov_forw(GLfloat distance)
{
	if (NO_CLIP)
	{
		if (collision(add_vec(cam.pos, mul_vec_scalar(cam.dir, distance))))	//u slucaju da je doslo do kolizije izadji iz funkcije i ne menjaj poziciju kamere cam.pos
			return;
		cam.pos = add_vec(cam.pos, mul_vec_scalar(cam.dir, distance));
	}
/*dva razlicita kretanja u zavisnosti od toga da li je NO_CLIP mod upaljen*/
	if (!NO_CLIP)
	{
		Vec3 horizontalCamDir=cam.dir;
		horizontalCamDir.y = 0;	//potrebno da bi se kretali u dve dimenzije kad je NO_CLIP ugasen
		
		if (collision(add_vec(cam.pos, mul_vec_scalar(horizontalCamDir, distance))))
			return;
		cam.pos = add_vec(cam.pos, mul_vec_scalar(horizontalCamDir, distance));
	}
}
/*da se ne bismo stalno kretali paralelno x osi globalnog koordinatnog sistema 
potrebno je da uzmemo u obzir gde trenutno gledamo i nadjemo upravan vektor na taj vector (camera direction) i up vector*/
void mov_right(GLfloat distance)
{
	Vec3 res = normalize_vec3(crossproduct(cam.dir, cam.upVec));

	if (collision(add_vec(cam.pos, mul_vec_scalar(res, distance))))
		return;

	cam.pos = add_vec(cam.pos, mul_vec_scalar(res, distance));
}
/*naredbu navedenu kod komentara iznad ne cinimo za kretanje na gore, jer ne zelimo da se "gore" menja u zavisnosti od toga gde kamera gleda*/
void mov_up(GLfloat distance)
{
	if (collision(add_vec(cam.pos, mul_vec_scalar(cam.upVec, distance))))
		return;
	cam.pos = add_vec(cam.pos, mul_vec_scalar(cam.upVec, distance));
}
/* funkcija implementira cucanje pormenom polozaja y kordinate pozicije kamere cam.pos
u zavisnosti od parametra mode, palimo ili gasimo cucanje*/
void crouch(int mode)
{
	if (mode)
	{
		cam.height -= 1.5f;
		cam.pos.y -= 1.5f;
		cam.speedMultiplier = 0.55f;
		cam.crouched = 1;
		KEY_TAB = 0;	// gasimo sprint kretanje kad smo u cucnju
	}
	else if (!mode)
	{
		cam.height += 1.5f;
		cam.pos.y += 1.5f;
		cam.speedMultiplier = 1.0f;
		cam.crouched = 0;
	}
}

void jump() 
{
	cam.jumpVelocity += grav.accel;
	if (cam.jumpVelocity < 0)
		cam.jumpVelocity = 0;
	
	cam.pos.y += cam.jumpVelocity;

	if (collision(add_vec(cam.pos, mul_vec_scalar(cam.upVec, cam.jumpVelocity)))) // u slucaju da skok dovodi do kolizije npr sa plafonom postavi y brzinu na 0  
	{
		cam.jumpVelocity = 0;
		return;
	}
}
/*funkcija je trebala da se ikoristi za drugi tip kretanja
ali je iskoriscena za blink kretanje,
ovo kretanje moze mnogo efikasnije da se implementira
ali kod nije diran jer radi to sto treba
mod sluzi za odredjivanje da li se "blink-ujemo" napred ili nazad*/
void blink_forw(float distance, float decelleration, int mode)
{
		decelleration = decelleration * 1.1f;
		distance += decelleration;

		if (distance < 0 && mode)
		{
			cam.spellCooldown = 5;
			return;
		}
		else if (distance > 0 && !mode)
		{
			cam.spellCooldown = 5;
			return;
		}

		mov_forw(distance);
		blink_forw(distance, decelleration, mode);
}

void blink_right(float distance, float decelleration, int mode)
{
		decelleration = decelleration * 1.1f;
		distance += decelleration;

		if (distance < 0 && mode)
		{
			cam.spellCooldown = 5;
			return;
		}

		if (distance > 0 && !mode)
		{
			cam.spellCooldown = 5;
			return;
		}
		mov_right(distance);
		blink_right(distance, decelleration, mode);
}
/*teleportacija se vrsi pozivanjem kretanja napred za razdaljinu 
TELEPORT_DISTANCE_SEGMENTS sve dok se ne naidje na koliziju,
kada dolazi do kolizije vrsi se jednokratno pomeranja igraca
za razdaljinu koja je suma svih poziva kretanja napred */
void teleport()
{
	Vec3 camPos=cam.pos;
	float totalDistance = 0.0f;

	while (1) 
	{
		if (collision(add_vec(camPos, mul_vec_scalar(cam.dir, TELEPORT_DISTANCE_SEGMENTS))) || totalDistance>200.0f)
		{
			cam.spellCooldown = 5;
			cam.pos = camPos;
			return;
		}
		totalDistance += TELEPORT_DISTANCE_SEGMENTS;
		camPos = add_vec(camPos, mul_vec_scalar(cam.dir, TELEPORT_DISTANCE_SEGMENTS));
	}
}
/*jetpack kretanje je prakticno kretanje kamere kao u NO_CLIP modu, 
samo sa vremenskim ogranicenjem zadatim cam.powerCharge vrednoscu i manjom brzinom*/
void jetpack(float distance)
{
		if (collision(add_vec(cam.pos, mul_vec_scalar(cam.dir, distance))))
			return;

		cam.pos = add_vec(cam.pos, mul_vec_scalar(cam.dir, distance));
		cam.powerCharge--;
}
/*poziva se u svakom tiku timer funkcije i povecava brzinu pada za grav.accel*/
void gravity() {
	grav.velocity += grav.accel;
	if (grav.velocity < grav.maxVelocity)
		grav.velocity = grav.maxVelocity;

	if (collision(add_vec(cam.pos, mul_vec_scalar(cam.upVec, grav.velocity)))) //u slucaju kolizije postavljamo brzinu -y na 0
	{
		grav.velocity = 0;													
		return;
	}
	cam.pos = add_vec(cam.pos, mul_vec_scalar(cam.upVec, grav.velocity));
}
/*proveravamo pitch da se kamera ne bih flipovala kad gledamo u nebo*/
void check_if_pitch_89()
{
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
}

void check_fov_val() 
{
	if (cam.fov > 75.0f)
		cam.fov = 75.0f;
	if (cam.fov < 45.0f)
		cam.fov = 45.0f;
}
/*bazicna animacija kamere koja se poziva kada se igrac spusti na 0 helta*/
void death_cam()
{
	if (viewPoint.x>0)
		viewPoint.x -= 0.10f;
	else if(viewPoint.x<0)
		viewPoint.x += 0.10f;

	if(viewPoint.y > 0)
		viewPoint.y -= 0.10f;
	else if (viewPoint.y < 0)
		viewPoint.y += 0.10f;

	if(viewPoint.z > 0)
		viewPoint.z -= 0.10f;
	else if (viewPoint.z < 0)
		viewPoint.z += 0.10f;

	if (viewPoint.x < 1 && viewPoint.x>-1)
		viewPoint.x = 0;
	if (viewPoint.y < 1 && viewPoint.y>-1)
		viewPoint.y = 0;
	if (viewPoint.z < 1 && viewPoint.z>-1)
		viewPoint.z = 0;

	gluLookAt(cam.pos.x, cam.pos.y, cam.pos.z,
			viewPoint.x, viewPoint.y, viewPoint.z,
			cam.upVec.x, cam.upVec.y, cam.upVec.z);

	if (!cam.crouched)
		crouch(1);
}
