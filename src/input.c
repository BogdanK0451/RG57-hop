#include <GL/glut.h>

#include<stdio.h>

#include "input.h"
#include "camera.h"
#include "loader.h"
#include "collision.h"
#include "draw.h"




float lastX = WINDOW_WIDTH / 2;
float lastY = WINDOW_HEIGHT / 2;

static int KEY_W = 0;
static int KEY_A = 0;
static int KEY_S = 0;
static int KEY_D = 0;

static int KEY_E = 0;
static int KEY_T = 0;
static int KEY_C = 0;
static int KEY_SPACE = 0;
static int KEY_C_CROUCH = 0;
int KEY_TAB = 0;	//sprint


static int LEFT_ARROW = 0;
static int RIGHT_ARROW = 0;
static int UP_ARROW = 0;
static int DOWN_ARROW = 0;
static int RIGHT_CLICK = 0;
static int MIDDLE_CLICK = 0;

static int FULLSCREEN = 0;		//F11 keybind
int MULTIPLE_VIEWPORTS = 0;		//F8 keybind
int TEXTURELESS_MODE = 0;		//F7 keybind
int DAY = 1;					//F6 keybind
int NO_CLIP = 0;				//F10 keybind  omogucava slobodno letenje po sceni, ukoliko se NO_CLIP ugasi, a igrac se nalazi unutar nekog objekta, poziva se get_unstuck da izbavi igraca
int SHOW_BOUNDARIES = 0;		//F9 keybind pokazuje kocku za koliziju
int RENDER_MODE = 2;			//7,8,9 keybinds RENDER_MODE GL_POINTS GL_LINES GL_FILL
/*mov_control se poziva u svakoj iteraciji timer funkcije 
(u nasem slucaju na svakih 16ms) i poziva razlicite funkcije kretanja
u zavisnosti od stanja dugmica npr WASD SPACE itd itd*/
void mov_control()
{
	if (NO_CLIP)
	{
		if (KEY_W)
			mov_forw(4 * MOVEMENT_SPEED);
		if (KEY_S)
			mov_forw(4 * -MOVEMENT_SPEED);
		if (KEY_A)
			mov_right(4 * -MOVEMENT_SPEED);
		if (KEY_D)
			mov_right(4 * MOVEMENT_SPEED);

		if (KEY_SPACE)
			mov_up(4 * MOVEMENT_SPEED);
		if (KEY_C)
			mov_up(4 * -MOVEMENT_SPEED);
	}
	if (!NO_CLIP)
	{
		if (MIDDLE_CLICK == 1 && !cam.spellCooldown)
		{
			if (KEY_W)
				blink_forw(BLINK_DISTANCE, DECELLERATION, 1);
			if (KEY_S)
				blink_forw(-BLINK_DISTANCE, -DECELLERATION, 0);
			if (KEY_A)
				blink_right(-BLINK_DISTANCE, -DECELLERATION, 0);
			if (KEY_D)
				blink_right(BLINK_DISTANCE, DECELLERATION, 1);
		}
		else
		{
			if (KEY_E)
			{
				if (cam.powerCharge > 0)
					jetpack(MOVEMENT_SPEED);
				else
					KEY_E = 0;
			}
			else if (!KEY_E)
			{
				if (KEY_W)
					mov_forw(cam.speedMultiplier*MOVEMENT_SPEED);
				if (KEY_S)
					mov_forw(cam.speedMultiplier*-MOVEMENT_SPEED);
				if (KEY_A)
					mov_right(cam.speedMultiplier*-MOVEMENT_SPEED);
				if (KEY_D)
					mov_right(cam.speedMultiplier*MOVEMENT_SPEED);

				if (KEY_T && !cam.spellCooldown)
				{
					teleport();
				}
				if (cam.jumpVelocity > 0 && cam.onGround)	// logika kod skakanja je pogresna jer bi zbog uslova cam.onGround==1 
					jump();									//	skok prilikom pritiska SPACE trebao da se pozove samo jednom i kamera da se pomeri za y=0.8, 
															//igrom slucaja gravitacija vraca cam.onGround na 1 i skakanje radi ali ovo treba da se reimplementira	
				gravity();
			}
		}
	}

	if (UP_ARROW)
	{
		pitch += -2.5f;
		check_if_pitch_89();
		cam_control(pitch, yaw);
	}
	if (DOWN_ARROW)
	{
		pitch += 2.5f;
		check_if_pitch_89();
		cam_control(pitch, yaw);
	}
	if (LEFT_ARROW)
	{
		yaw += -2.5f;
		cam_control(pitch, yaw);
	}
	if (RIGHT_ARROW)
	{
		yaw += 2.5f;
		cam_control(pitch, yaw);
	}
	if (RIGHT_CLICK)
	{
		cam.fov += -1;
		check_fov_val();
	}
	if (!RIGHT_CLICK)
	{
		cam.fov += 1;
		check_fov_val();
	}
}
/*postavljamo state-ove promenljivih i pozivamo funkcije
u zavisnosti od trenutno pritisnutih dugmica*/
void on_keyboard(unsigned char key, int x, int y)
{
	switch (key) 
	{
	case ESC:
		for (int i = 0; i < objNum; i++)		//oslobadanje alocirane memorije za objkete
		{
			free(obj[i].v);
			free(obj[i].vt);
			free(obj[i].vn);
			free(obj[i].faces);
		}
		exit(EXIT_SUCCESS);
		break;
	case 'W':
	case 'w':
		KEY_W = 1;
		break;
	case 'S':
	case 's':
		KEY_S = 1;
		break;
	case 'D':
	case 'd':
		KEY_D = 1;
		break;
	case 'A':
	case 'a':
		KEY_A = 1;
		break;
	case 'C':
	case 'c':
		if (NO_CLIP)
			KEY_C = 1;
		else if (!NO_CLIP)
		{
			if (!KEY_C_CROUCH)
			{
				KEY_C_CROUCH = 1;
				if (KEY_C_CROUCH && (!cam.crouched) && cam.health)
					crouch(1);
				else if (KEY_C_CROUCH && cam.crouched && cam.health)
					crouch(0);
			}
		}
		break;
	case 'E':
	case 'e':
		KEY_E = 1;
		break;
	case 'T':
	case 't':
		KEY_T = 1;
		break;
	case TAB:
		if (!KEY_TAB)
		{
			KEY_TAB = 1;
			if (KEY_TAB && !(KEY_C_CROUCH) && cam.speedMultiplier==1.0f)		//sprint
				cam.speedMultiplier = 1.5f;
			else if (KEY_TAB && !(KEY_C_CROUCH) && cam.speedMultiplier==1.5f)
				cam.speedMultiplier = 1.0f;
		}
		break;
	case SPACE:
		if (NO_CLIP) 
			KEY_SPACE = 1;
		else if (!NO_CLIP && cam.onGround && !KEY_SPACE)
		{
			cam.onGround = 0;
			cam.jumpVelocity = 0.8f;
			KEY_SPACE = 1;
		}
		break;
	case '7':
		RENDER_MODE = 0; // glPolygonMode GL_POINTS
		break;
	case '8':
		RENDER_MODE = 1; // glPolygonMode GL_LINES
		break;
	case '9':
		RENDER_MODE = 2; // glPolygonMode GL_FILL
		break;
	}
}
/*isto kao on_keyboard() ali pri otpustanju dugmica*/
void on_keyboard_up(unsigned char key, int x, int y) 
{
	switch (key)
	{
	case 'W':
	case 'w':
		KEY_W = 0;
		break;
	case 'S':
	case 's':
		KEY_S = 0;
		break;
	case 'D':
	case 'd':
		KEY_D = 0;
		break;
	case 'A':
	case 'a':
		KEY_A = 0;
		break;
	case 'C':
	case 'c':
		if (NO_CLIP)
			KEY_C = 0;
		else if (!NO_CLIP)
			KEY_C_CROUCH = 0;
		break;
	case 'E':
	case 'e':
		KEY_E = 0;
		break;
	case 'T':
	case 't':
		KEY_T = 0;
		break;
	case TAB:
		KEY_TAB = 0;
		break;
	case SPACE:
		if(NO_CLIP)
			KEY_SPACE = 0;
		else if (!NO_CLIP && KEY_SPACE)
		{
			if (cam.jumpVelocity > 0.4f)
				cam.jumpVelocity = 0.4f;
			KEY_SPACE = 0;
		}
		break;
	}
}
/*poziva se pri pomeranju misa
sluzi za dobijanje delte pozicije misa koju koristimo za pomeranje kamere*/
void on_mouse_mov(int xPos, int yPos)
{
	int xCenter = glutGet(GLUT_WINDOW_WIDTH) / 2;
	int yCenter = glutGet(GLUT_WINDOW_HEIGHT) / 2;

	static int warped = 0;

	//pomeranje cursora glutWarpPointerom-om poziva callback funkciju on_mouse_mov, to sprecavamo dole navedenim uslovom
	if (warped)
	{
		warped = 0;
		return;
	}
	warped = 1;
	glutWarpPointer(xCenter, yCenter);

	float xOffset = (float)xPos - xCenter;
	float yOffset = (float)yPos - yCenter;
	float sensitivity = 0.10f;

	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;

	check_if_pitch_89();
}

/* isto kao on_mouse_mov() al se poziva kad je neko dugme misa pritisnuto*/
void on_mouse_mov_and_click(int xPos, int yPos)
{
	on_mouse_mov(xPos,yPos);
}
void on_mouse_click(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		RIGHT_CLICK = 1;
	else if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
		RIGHT_CLICK = 0;

	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
		MIDDLE_CLICK = 1;
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
		MIDDLE_CLICK = 0;

}
/*isto kao*/
void on_special_keyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_RIGHT:
		RIGHT_ARROW = 1;
		break;
	case GLUT_KEY_LEFT:
		LEFT_ARROW = 1;
		break;
	case GLUT_KEY_UP:
		UP_ARROW = 1;
		break;
	case GLUT_KEY_DOWN:
		DOWN_ARROW = 1;
		break;
	case GLUT_KEY_F11:
		if (!FULLSCREEN)
		{
			glutFullScreen();
			FULLSCREEN = 1;
		}
		else {
			glutPositionWindow(100, 0);
			FULLSCREEN = 0;
		}
		break;
	case GLUT_KEY_F10:
		if (NO_CLIP)
		{
			NO_CLIP = 0;
			get_unstuck();
		}
		else if (!NO_CLIP && cam.health) 
		{
			grav.velocity = 0.0f;
			cam.onGround = 0;
			for (int i = 0; i < objNum; i++)
				obj[i].collided = 0;

			NO_CLIP = 1;
		}
		break;
	case GLUT_KEY_F9:
		if (!SHOW_BOUNDARIES)
		{
			SHOW_BOUNDARIES = 1;
		}
		else 
		{
			SHOW_BOUNDARIES = 0;
		}
		break;
	case GLUT_KEY_F8:
		if (!MULTIPLE_VIEWPORTS)
		{
			MULTIPLE_VIEWPORTS = 1;
		}
		else {
			RENDER_MODE = 2;
			MULTIPLE_VIEWPORTS = 0;
		}
		break;
	case GLUT_KEY_F7:
		if (!TEXTURELESS_MODE)
		{
			TEXTURELESS_MODE = 1;
			SHOW_BOUNDARIES = 1; //kad se upali textureless mode nema osvetljenja pa je "tesko" snaci se u prostoru, zato se pali SHOW_BOUNDARIES i prikazuju outlineovi objekata
		}
		else 
		{
			TEXTURELESS_MODE = 0;
		}
		break;
	case GLUT_KEY_F6:
		if (DAY)
			DAY = 0;
		else if (!DAY)
			DAY = 1;
		break;
	case GLUT_KEY_F5:	//restartujemo igru 
			cam.health = 100;		
			cam.pos.x = 11;
			cam.pos.y = 10;
			cam.pos.z = 10;
			crouch(0);
		break;
	case GLUT_KEY_PAGE_UP:
		effect1Speed++;	//ubrzavamo iscrtavanje objekta
		break;
	case GLUT_KEY_PAGE_DOWN:
		effect1Speed--;
		break;
	}
}

void on_special_keyboard_up(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_RIGHT:
		RIGHT_ARROW = 0;
		break;
	case GLUT_KEY_LEFT:
		LEFT_ARROW = 0;
		break;
	case GLUT_KEY_UP:
		UP_ARROW = 0;
		break;
	case GLUT_KEY_DOWN:
		DOWN_ARROW = 0;
		break;
	}
}