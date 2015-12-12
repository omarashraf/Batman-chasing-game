#include <vector>
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <Windows.h>
#include <stdlib.h> 
#include <stdio.h>



// old variables
float x_rpg = 27;
bool bullet = false;
float rpg_range = 0;
bool out = false;

bool shoot2 = false;
float rot_angle_joker_car_left = 0;
float rot_angle_joker_left = 0;

bool escaped = false;
float escape_counter = 0;
int time = 0;

float rot_angle_joker = 0;
float rot_angle_joker_car_right = 0;
float x_joker_factor = 0;

int dir = -1;
int lane = 3;
int predicted_lane;
int batman_lane = 3;
int counter = 1;

float batmobile_length = 11;
float joker_car_length = 10;

bool hit = false;
bool shoot = false;
int hit_range = 0;

float x_joker = 28;
float z_joker = 15;

float x_joker_car = 30;

bool jLeft = false;
bool jRight = false;

float z_end_left_joker;
float z_end_right_joker;

float x_step = 0;
float z_step = 15;
float z_end_left;
float z_end_right;
float x_camera = -20.0f;

bool move_right = false;
bool move_left = false;
bool paused = false;

int WIDTH = 1280;
int HEIGHT = 720;

char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 100;

int cameraZoom = 0;

// Model Variables
Model_3DS model_batmobile;
Model_3DS model_joker_car;
Model_3DS model_joker;

// Textures
GLTexture tex_ground;
GLTexture tex_service_lane;
GLTexture tex_street;
GLTexture tex_wall;
GLTexture tex_cave;
GLTexture tex_grass;
GLTexture tex_poster;
GLTexture tex_poster2;
GLTexture tex_poster_batman;
GLTexture tex_poster2_batman;
GLTexture tex_sidewalk;
GLTexture tex_building;
GLTexture tex_building2;
GLTexture tex_building4;
GLTexture tex_building3;
GLTexture tex_building5;
GLTexture tex_building6;
GLTexture tex_building7;
GLTexture tex_building8;

// Bullets class

class Bullets{ //new
public:
	float xin, x, z;
	int pos;

	Bullets(float x, float z, int pos) {
		this->xin = x;
		this->z = z;
		this->x = x;
		this->pos = pos;
	}
};

std::vector<Bullets> bullets; //new
bool gameover; //new

void DrawBullets() { //new: Displaying the bullets
	if (bullets.size()>0) {
		for (int i = 0; i<bullets.size(); i++)
		{
			glPushMatrix();
			glColor3f(0.0f, 1.0f, 1.0f);
			glTranslated(bullets[i].x + 0.75, 4, bullets[i].z);
			glutSolidSphere(0.5, 10, 10);
			glPopMatrix();
		}
	}
}

void check() { //new: check if a bullet has hit the batmobile
	if (bullets.size()>0) {
		for (int i = 0; i<bullets.size(); i++) {
			if (bullets[i].pos == batman_lane && bullets[i].x <= x_step + 2 && x_step - 2 <= bullets[i].x) {
				bullets.clear();
				gameover = true;
				break;
			}
			else {
				gameover = false;
			}
		}
	}
}

void MoveBullets() { //new: change each bullet's xdirection
	if (bullets.size()>0) {
		for (int i = 0; i < bullets.size(); i++) {
			if (bullets[i].xin - bullets[i].x >= 30) {
				bullets.erase(bullets.begin() + i);
			}
			else
				bullets[i].x -= 0.02;
		}
	}
}

// print to the screen
void print(char *string, float x, float y, float z)
{
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	int len;
	glRasterPos3f(x, y, z);
	glColor3f(1.0, 1.0, 1.0);
	len = (int)strlen(string);
	for (int i = 0; i<len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(-20.0f, 20.0f, 15.0f, 20.0f, 10.0f, 15.0f, 0.0f, 1.0f, 0.0f);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground and Walls Function
//=======================================================================
void RenderWalls(float x, float y, float z, GLTexture tex) {
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(x, 0, z);
	glTexCoord2f(1, 0);
	glVertex3f(x, y, z);
	glTexCoord2f(1, 1);
	glVertex3f(x + 50, y, z);
	glTexCoord2f(0, 1);
	glVertex3f(x + 50, 0, z);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void RenderWallsHelper(float x, float z, GLTexture tex) {
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex.texture[0]);	// Bind the ground texture

	int z1, z2;
	if (z == 0) {
		z1 = 0;
		z2 = -30;
	}
	else {
		z1 = 30;
		z2 = 60;
	}

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(x, 0, z1);
	glTexCoord2f(1, 0);
	glVertex3f(x, 20, z1);
	glTexCoord2f(1, 1);
	glVertex3f(x, 20, z2);
	glTexCoord2f(0, 1);
	glVertex3f(x, 0, z2);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void renderPoster(float x, float y1, float y2, float z, GLTexture tex) {
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(x, y1, z);
	glTexCoord2f(1, 0);
	glVertex3f(x, y2, z);
	glTexCoord2f(1, 1);
	glVertex3f(x + 5, y2, z);
	glTexCoord2f(0, 1);
	glVertex3f(x + 5, y1, z);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-10, 0, 0);
	glTexCoord2f(5, 0);
	glVertex3f(1300, 0, 0);
	glTexCoord2f(5, 5);
	glVertex3f(1300, 0, 30);
	glTexCoord2f(0, 5);
	glVertex3f(-10, 0, 30);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void RenderSideWalk(float x1, float x2, float z1, float z2)
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_sidewalk.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(x1, 1.5, z1);
	glTexCoord2f(1, 0);
	glVertex3f(x2, 1.5, z1);
	glTexCoord2f(1, 1);
	glVertex3f(x2, 1.5, z2);
	glTexCoord2f(0, 1);
	glVertex3f(x1, 1.5, z2);
	glEnd();
	glPopMatrix();

	float z;
	if (z1 == 0) {
		z = 3;
	}
	else {
		z = 27;
	}
	
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(x1, 1.5, z);
	glTexCoord2f(1, 0);
	glVertex3f(x2, 1.5, z);
	glTexCoord2f(1, 1);
	glVertex3f(x2, 0.0, z);
	glTexCoord2f(0, 1);
	glVertex3f(x1, 0.0, z);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void renderSidewalkHelper(float x) {
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_building.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glColor3f(1, 0, 0);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(x, 0.1, 10);
	glTexCoord2f(1, 0);
	glVertex3f(x, 5, 15);
	glTexCoord2f(1, 1);
	glVertex3f(x, 5, 15);
	glTexCoord2f(0, 1);
	glVertex3f(x, 0.1, 10);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void renderStreet(float x1, float x2, float z1, float z2)
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_street.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(x1, 0, z1);
	glTexCoord2f(1, 0);
	glVertex3f(x2, 0, z1);
	glTexCoord2f(1, 1);
	glVertex3f(x2, 0, z2);
	glTexCoord2f(0, 1);
	glVertex3f(x1, 0, z2);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void lChangeValue(int tmp_lane) {
	switch (tmp_lane) {
	case 1: z_end_left_joker = 4.5; lane = 1; break;
	case 2: z_end_left_joker = 10; lane = 2; break;
	case 3: z_end_left_joker = 15; lane = 3; break;
	case 4: z_end_left_joker = 20; lane = 4; break;
	}
}

void rChangeValue(int tmp_lane) {
	switch (tmp_lane) {
	case 2: z_end_right_joker = 10; lane = 2; break;
	case 3: z_end_right_joker = 15; lane = 3; break;
	case 4: z_end_right_joker = 20; lane = 4; break;
	case 5: z_end_right_joker = 25.5; lane = 5; break;
	}
}

// random joker movement
void jChange() {
	dir = rand() % 2;
	predicted_lane = (rand() % 5) + 1;

	if (dir == 0 && !jRight && !jLeft && predicted_lane < lane) {
		if (predicted_lane > batman_lane) {
			lChangeValue(predicted_lane);
			//bullets.push_back(Bullets(x_joker, z_joker, lane)); //new: adding bullets
			jLeft = true;
		}
		else {
			if (x_step + (batmobile_length / 2) - 5.6 <= x_joker - (joker_car_length / 2) && !hit) {
				lChangeValue(predicted_lane);
				//bullets.push_back(Bullets(x_joker, z_joker, lane)); //new: adding bullets
				jLeft = true;
			}
		}
	}

	if (dir == 1 && !jRight && !jLeft && predicted_lane > lane) {
		if (predicted_lane < batman_lane) {
			rChangeValue(predicted_lane);
			jRight = true;
		}
		else {
			if (x_step + (batmobile_length / 2) - 5.6 <= x_joker - (joker_car_length / 2) && !hit) {
				rChangeValue(predicted_lane);
				jRight = true;
			}
		}
	}
}

// Animation function
void Anim() {
	check();
	counter += 1;
	if (gameover) {
		escaped = true;
		gameover = false;
	}
	else {
		if (escaped && !shoot && !shoot2) {
			// joker escapes plus his laugh
			escape_counter += 1;
			x_joker += 0.25;
			x_joker_car += 0.25;
			x_rpg += 0.25;
		}
		else {
			if (!shoot && !shoot2) {
				if (bullet) {
					if (rpg_range >= 3.3 && !out) {
						//bullet = false;
						out = true;
						rpg_range = 3.3;
					}
					else {
						if (rpg_range < 0 && out) {
							bullet = false;
							rpg_range = 0;
							out = false;
						}
					}
					if (out) {
						rpg_range -= 0.1;
					}
					else {
						rpg_range += 0.1;
					}
					
				}
				if (!hit) {
					MoveBullets(); //new: changing x dir & removing bullets after el dis betwen initial position and now postion is greater than 10
					x_joker += 0.18;
					x_joker_car += 0.18;
					x_rpg += 0.18;
				}
				else {
					x_joker += 0.3;
					x_joker_car += 0.3;
					x_rpg += 0.3;
					x_step -= 0.1;
					x_camera -= 0.1;
					hit_range--;
					if (hit_range <= 0) {
						hit = false;
					}
				}

				if (jLeft) {
					z_joker -= 0.1;
					if (z_joker < z_end_left_joker) {
						jLeft = false;
						z_joker = z_end_left_joker;
					}
				}
				else {
					if (jRight) {
						z_joker += 0.1;
						if (z_joker > z_end_right_joker) {
							jRight = false;
							z_joker = z_end_right_joker;
						}
					}
				}

				if (!move_left && !move_right && !hit) {
					x_step += 0.206;
					x_camera += 0.206;
				}
				else {
					if (move_right) {
						if (z_step < z_end_right) {
							z_step += 0.1;
						}
						else {
							z_step = z_end_right;
							move_right = false;
						}
					}

					if (move_left) {
						if (z_step > z_end_left) {
							z_step -= 0.1;
						}
						else {
							z_step = z_end_left;
							move_left = false;
						}
					}
					// to be changed to give the delay effect
					x_step += 0.1;
					x_camera += 0.1;
				}
			}
			else {
				if (shoot) {
					rot_angle_joker -= 1;
					rot_angle_joker_car_right += 1;
					x_joker_factor += 0.03;
					x_joker += 0.09;
					x_joker_car += 0.09;
					x_rpg += 0.09;
				}
				else {
					if (shoot2) {
						rot_angle_joker_left += 1;
						rot_angle_joker_car_left -= 1;
						x_joker_factor += 0.03;
						x_joker += 0.09;
						x_joker_car += 0.09;
						x_rpg += 0.09;
					}
				}

			}
		}
	}
	
	
	time += 0.5;
	glutPostRedisplay();
}

void drawRect(float x, float y, float w, float h, GLTexture tex) {
	glBindTexture(GL_TEXTURE_2D, tex.texture[0]);	// Bind the ground texture
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, 1);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + w, y, 1);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + w, y + h, 1);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + h, 1);
	glEnd();
}

void drawRect(float x, float y, float w, float h) {
	//glBindTexture(GL_TEXTURE_2D, tex.texture[0]);	// Bind the ground texture
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, 1);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + w, y, 1);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + w, y + h, 1);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + h, 1);
	glEnd();
}

void drawBox(GLTexture tex) {
	glPushMatrix();

	glPushMatrix();
	drawRect(-1, -1, 2, 2, tex);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	drawRect(-1, -1, 2, 2, tex);
	glPopMatrix();

	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	drawRect(-1, -1, 2, 2, tex);
	glPopMatrix();

	glPushMatrix();
	glRotatef(270, 0, 1, 0);
	drawRect(-1, -1, 2, 2, tex);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	drawRect(-1, -1, 2, 2, tex);
	glPopMatrix();

	glPushMatrix();
	glRotatef(270, 1, 0, 0);
	drawRect(-1, -1, 2, 2, tex);
	glPopMatrix();

	glPopMatrix();
}

void drawBox() {
	glPushMatrix();

	glPushMatrix();
	drawRect(-1, -1, 2, 2);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	drawRect(-1, -1, 2, 2);
	glPopMatrix();

	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	drawRect(-1, -1, 2, 2);
	glPopMatrix();

	glPushMatrix();
	glRotatef(270, 0, 1, 0);
	drawRect(-1, -1, 2, 2);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	drawRect(-1, -1, 2, 2);
	glPopMatrix();

	glPushMatrix();
	glRotatef(270, 1, 0, 0);
	drawRect(-1, -1, 2, 2);
	glPopMatrix();

	glPopMatrix();
}

void sidewalk() {
	glPushMatrix();
	glTranslated(0, 0, -6);
	RenderSideWalk(-10, 0, 0, 3);
	RenderSideWalk(0, 10, 0, 3);
	RenderSideWalk(10, 20, 0, 3);
	RenderSideWalk(20, 30, 0, 3);
	RenderSideWalk(30, 40, 0, 3);
	RenderSideWalk(40, 50, 0, 3);
	RenderSideWalk(50, 60, 0, 3);
	RenderSideWalk(60, 70, 0, 3);
	RenderSideWalk(70, 80, 0, 3);
	RenderSideWalk(80, 90, 0, 3);
	RenderSideWalk(90, 100, 0, 3);
	RenderSideWalk(100, 110, 0, 3);
	RenderSideWalk(110, 120, 0, 3);
	RenderSideWalk(120, 130, 0, 3);
	RenderSideWalk(130, 140, 0, 3);
	RenderSideWalk(140, 150, 0, 3);
	RenderSideWalk(150, 160, 0, 3);
	RenderSideWalk(160, 170, 0, 3);
	RenderSideWalk(170, 180, 0, 3);
	RenderSideWalk(180, 190, 0, 3);
	RenderSideWalk(190, 200, 0, 3);
	RenderSideWalk(200, 210, 0, 3);
	RenderSideWalk(210, 220, 0, 3);
	RenderSideWalk(220, 230, 0, 3);
	RenderSideWalk(230, 240, 0, 3);
	RenderSideWalk(240, 250, 0, 3);
	RenderSideWalk(250, 260, 0, 3);
	RenderSideWalk(260, 270, 0, 3);
	RenderSideWalk(270, 280, 0, 3);
	RenderSideWalk(280, 287, 0, 3);
	renderStreet(287, 310, 3, -20);
	glPushMatrix();
	glBegin(GL_POLYGON);
	glColor3f(1, 1, 1);
	glVertex3f(310, 0, 3);
	glVertex3f(310, 1.5, 3);
	glVertex3f(310, 1.5, 0);
	glVertex3f(310, 0, 0);
	glEnd();
	glPopMatrix();
	RenderSideWalk(310, 320, 0, 3);
	RenderSideWalk(320, 330, 0, 3);
	RenderSideWalk(330, 340, 0, 3);
	RenderSideWalk(340, 350, 0, 3);
	RenderSideWalk(350, 360, 0, 3);
	RenderSideWalk(360, 370, 0, 3);
	RenderSideWalk(370, 380, 0, 3);
	RenderSideWalk(380, 390, 0, 3);
	RenderSideWalk(390, 400, 0, 3);
	RenderSideWalk(400, 410, 0, 3);
	RenderSideWalk(410, 420, 0, 3);
	RenderSideWalk(420, 430, 0, 3);
	RenderSideWalk(430, 440, 0, 3);
	RenderSideWalk(440, 450, 0, 3);
	RenderSideWalk(450, 460, 0, 3);
	RenderSideWalk(460, 470, 0, 3);
	RenderSideWalk(470, 480, 0, 3);
	RenderSideWalk(480, 490, 0, 3);
	RenderSideWalk(490, 500, 0, 3);
	RenderSideWalk(500, 510, 0, 3);
	RenderSideWalk(510, 520, 0, 3);
	RenderSideWalk(520, 530, 0, 3);
	RenderSideWalk(530, 540, 0, 3);
	RenderSideWalk(540, 550, 0, 3);
	RenderSideWalk(550, 560, 0, 3);
	RenderSideWalk(560, 570, 0, 3);
	RenderSideWalk(570, 580, 0, 3);
	RenderSideWalk(580, 590, 0, 3);
	RenderSideWalk(590, 600, 0, 3);
	RenderSideWalk(600, 610, 0, 3);
	RenderSideWalk(610, 620, 0, 3);
	RenderSideWalk(620, 630, 0, 3);
	RenderSideWalk(630, 640, 0, 3);
	RenderSideWalk(640, 650, 0, 3);
	RenderSideWalk(650, 660, 0, 3);
	RenderSideWalk(660, 670, 0, 3);
	RenderSideWalk(670, 680, 0, 3);
	RenderSideWalk(680, 690, 0, 3);
	RenderSideWalk(690, 700, 0, 3);
	RenderSideWalk(700, 710, 0, 3);
	RenderSideWalk(710, 720, 0, 3);
	RenderSideWalk(720, 730, 0, 3);
	RenderSideWalk(730, 740, 0, 3);
	RenderSideWalk(740, 750, 0, 3);
	RenderSideWalk(750, 760, 0, 3);
	RenderSideWalk(760, 770, 0, 3);
	RenderSideWalk(770, 780, 0, 3);
	RenderSideWalk(780, 790, 0, 3);
	RenderSideWalk(790, 800, 0, 3);
	RenderSideWalk(800, 810, 0, 3);
	RenderSideWalk(810, 820, 0, 3);
	RenderSideWalk(820, 830, 0, 3);
	RenderSideWalk(830, 840, 0, 3);
	RenderSideWalk(840, 850, 0, 3);
	RenderSideWalk(850, 860, 0, 3);
	RenderSideWalk(860, 870, 0, 3);
	RenderSideWalk(870, 880, 0, 3);
	RenderSideWalk(880, 890, 0, 3);
	RenderSideWalk(890, 900, 0, 3);
	RenderSideWalk(900, 910, 0, 3);
	RenderSideWalk(910, 920, 0, 3);
	RenderSideWalk(920, 930, 0, 3);
	RenderSideWalk(930, 940, 0, 3);
	RenderSideWalk(940, 950, 0, 3);
	RenderSideWalk(950, 960, 0, 3);
	RenderSideWalk(960, 970, 0, 3);
	RenderSideWalk(970, 980, 0, 3);
	RenderSideWalk(980, 990, 0, 3);
	RenderSideWalk(990, 1000, 0, 3);
	RenderSideWalk(1000, 1010, 0, 3);
	RenderSideWalk(1010, 1020, 0, 3);
	RenderSideWalk(1020, 1030, 0, 3);
	RenderSideWalk(1030, 1040, 0, 3);
	RenderSideWalk(1040, 1050, 0, 3);
	RenderSideWalk(1050, 1060, 0, 3);
	RenderSideWalk(1060, 1070, 0, 3);
	RenderSideWalk(1070, 1080, 0, 3);
	RenderSideWalk(1080, 1090, 0, 3);
	RenderSideWalk(1090, 1100, 0, 3);
	RenderSideWalk(1100, 1110, 0, 3);
	RenderSideWalk(1110, 1120, 0, 3);
	RenderSideWalk(1120, 1130, 0, 3);
	RenderSideWalk(1130, 1140, 0, 3);
	RenderSideWalk(1140, 1150, 0, 3);
	RenderSideWalk(1150, 1160, 0, 3);
	RenderSideWalk(1160, 1170, 0, 3);
	RenderSideWalk(1170, 1180, 0, 3);
	RenderSideWalk(1180, 1190, 0, 3);
	RenderSideWalk(1190, 1200, 0, 3);
	RenderSideWalk(1200, 1210, 0, 3);
	RenderSideWalk(1210, 1220, 0, 3);
	RenderSideWalk(1220, 1230, 0, 3);
	RenderSideWalk(1230, 1240, 0, 3);
	RenderSideWalk(1240, 1250, 0, 3);
	RenderSideWalk(1250, 1260, 0, 3);
	RenderSideWalk(1260, 1270, 0, 3);
	RenderSideWalk(1270, 1280, 0, 3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, 6);
	RenderSideWalk(-10, 0, 27, 30);
	RenderSideWalk(0, 10, 27, 30);
	RenderSideWalk(10, 20, 27, 30);
	RenderSideWalk(20, 30, 27, 30);
	RenderSideWalk(30, 36, 27, 30);
	renderStreet(36.1, 42.9, 27, 40);
	glPushMatrix();
	glBegin(GL_POLYGON);
	glColor3f(1, 1, 1);
	glVertex3f(42.9, 0, 27);
	glVertex3f(42.9, 1.5, 27);
	glVertex3f(42.9, 1.5, 30);
	glVertex3f(42.9, 0, 30);
	glEnd();
	glPopMatrix();
	RenderSideWalk(43, 50, 27, 30);
	RenderSideWalk(50, 60, 27, 30);
	RenderSideWalk(60, 70, 27, 30);
	RenderSideWalk(70, 80, 27, 30);
	RenderSideWalk(80, 90, 27, 30);
	RenderSideWalk(90, 100, 27, 30);
	RenderSideWalk(100, 110, 27, 30);
	RenderSideWalk(110, 120, 27, 30);
	RenderSideWalk(120, 130, 27, 30);
	RenderSideWalk(130, 140, 27, 30);
	RenderSideWalk(140, 150, 27, 30);
	RenderSideWalk(150, 160, 27, 30);
	RenderSideWalk(160, 170, 27, 30);
	RenderSideWalk(170, 180, 27, 30);
	RenderSideWalk(180, 190, 27, 30);
	RenderSideWalk(190, 200, 27, 30);
	RenderSideWalk(200, 210, 27, 30);
	RenderSideWalk(210, 220, 27, 30);
	RenderSideWalk(220, 230, 27, 30);
	RenderSideWalk(230, 240, 27, 30);
	RenderSideWalk(240, 250, 27, 30);
	RenderSideWalk(250, 260, 27, 30);
	RenderSideWalk(260, 270, 27, 30);
	RenderSideWalk(270, 280, 27, 30);
	RenderSideWalk(280, 290, 27, 30);
	RenderSideWalk(290, 300, 27, 30);
	RenderSideWalk(300, 310, 27, 30);
	RenderSideWalk(310, 320, 27, 30);
	RenderSideWalk(320, 330, 27, 30);
	RenderSideWalk(330, 340, 27, 30);
	RenderSideWalk(340, 350, 27, 30);
	RenderSideWalk(350, 360, 27, 30);
	RenderSideWalk(360, 370, 27, 30);
	RenderSideWalk(370, 380, 27, 30);
	RenderSideWalk(380, 390, 27, 30);
	RenderSideWalk(390, 400, 27, 30);
	RenderSideWalk(400, 410, 27, 30);
	RenderSideWalk(410, 420, 27, 30);
	RenderSideWalk(420, 430, 27, 30);
	RenderSideWalk(430, 440, 27, 30);
	RenderSideWalk(440, 450, 27, 30);
	RenderSideWalk(450, 460, 27, 30);
	RenderSideWalk(460, 470, 27, 30);
	RenderSideWalk(470, 480, 27, 30);
	RenderSideWalk(480, 490, 27, 30);
	RenderSideWalk(490, 500, 27, 30);
	RenderSideWalk(500, 510, 27, 30);
	RenderSideWalk(510, 520, 27, 30);
	RenderSideWalk(520, 530, 27, 30);
	RenderSideWalk(530, 540, 27, 30);
	RenderSideWalk(540, 550, 27, 30);
	RenderSideWalk(550, 560, 27, 30);
	RenderSideWalk(560, 570, 27, 30);
	RenderSideWalk(570, 580, 27, 30);
	RenderSideWalk(580, 590, 27, 30);
	RenderSideWalk(590, 600, 27, 30);
	RenderSideWalk(600, 610, 27, 30);
	RenderSideWalk(610, 620, 27, 30);
	RenderSideWalk(620, 630, 27, 30);
	RenderSideWalk(630, 640, 27, 30);
	RenderSideWalk(640, 650, 27, 30);
	RenderSideWalk(650, 660, 27, 30);
	RenderSideWalk(660, 670, 27, 30);
	RenderSideWalk(670, 680, 27, 30);
	RenderSideWalk(680, 690, 27, 30);
	RenderSideWalk(690, 700, 27, 30);
	RenderSideWalk(700, 710, 27, 30);
	RenderSideWalk(710, 720, 27, 30);
	RenderSideWalk(720, 730, 27, 30);
	RenderSideWalk(730, 740, 27, 30);
	RenderSideWalk(740, 750, 27, 30);
	RenderSideWalk(750, 760, 27, 30);
	RenderSideWalk(760, 770, 27, 30);
	RenderSideWalk(770, 780, 27, 30);
	RenderSideWalk(780, 790, 27, 30);
	RenderSideWalk(790, 800, 27, 30);
	RenderSideWalk(800, 810, 27, 30);
	RenderSideWalk(810, 820, 27, 30);
	RenderSideWalk(820, 830, 27, 30);
	RenderSideWalk(830, 840, 27, 30);
	RenderSideWalk(840, 850, 27, 30);
	RenderSideWalk(850, 860, 27, 30);
	RenderSideWalk(860, 870, 27, 30);
	RenderSideWalk(870, 880, 27, 30);
	RenderSideWalk(880, 890, 27, 30);
	RenderSideWalk(890, 900, 27, 30);
	RenderSideWalk(900, 910, 27, 30);
	RenderSideWalk(910, 920, 27, 30);
	RenderSideWalk(920, 930, 27, 30);
	RenderSideWalk(920, 930, 27, 30);
	RenderSideWalk(930, 940, 27, 30);
	RenderSideWalk(940, 950, 27, 30);
	RenderSideWalk(950, 960, 27, 30);
	RenderSideWalk(960, 970, 27, 30);
	RenderSideWalk(970, 980, 27, 30);
	RenderSideWalk(980, 990, 27, 30);
	RenderSideWalk(990, 1000, 27, 30);
	RenderSideWalk(1000, 1010, 27, 30);
	RenderSideWalk(1010, 1020, 27, 30);
	RenderSideWalk(1020, 1030, 27, 30);
	RenderSideWalk(1030, 1040, 27, 30);
	RenderSideWalk(1040, 1050, 27, 30);
	RenderSideWalk(1050, 1060, 27, 30);
	RenderSideWalk(1060, 1070, 27, 30);
	RenderSideWalk(1070, 1080, 27, 30);
	RenderSideWalk(1080, 1090, 27, 30);
	RenderSideWalk(1090, 1100, 27, 30);
	RenderSideWalk(1100, 1110, 27, 30);
	RenderSideWalk(1110, 1120, 27, 30);
	RenderSideWalk(1120, 1130, 27, 30);
	RenderSideWalk(1130, 1140, 27, 30);
	RenderSideWalk(1140, 1150, 27, 30);
	RenderSideWalk(1150, 1160, 27, 30);
	RenderSideWalk(1160, 1170, 27, 30);
	RenderSideWalk(1170, 1180, 27, 30);
	RenderSideWalk(1180, 1190, 27, 30);
	RenderSideWalk(1190, 1200, 27, 30);
	RenderSideWalk(1200, 1210, 27, 30);
	RenderSideWalk(1210, 1220, 27, 30);
	RenderSideWalk(1220, 1230, 27, 30);
	RenderSideWalk(1230, 1240, 27, 30);
	RenderSideWalk(1240, 1250, 27, 30);
	RenderSideWalk(1250, 1260, 27, 30);
	RenderSideWalk(1260, 1270, 27, 30);
	RenderSideWalk(1270, 1280, 27, 30);
	glPopMatrix();
}

void walls() {

	// Draw left wall
	glPushMatrix();
	glTranslated(0, 0, -6);
	RenderWallsHelper(87, 0, tex_wall);
	RenderWalls(87, 20, 0, tex_wall);
	RenderWalls(137, 20, 0, tex_wall);
	RenderWalls(187, 20, 0, tex_wall);
	RenderWalls(237, 20, 0, tex_wall);
	RenderWallsHelper(510, 0, tex_cave);
	RenderWalls(510, 20, 0, tex_cave);
	RenderWalls(560, 20, 0, tex_cave);
	RenderWalls(610, 20, 0, tex_cave);
	RenderWalls(660, 20, 0, tex_cave);
	RenderWallsHelper(900, 0, tex_grass);
	RenderWalls(900, 20, 0, tex_grass);
	RenderWalls(950, 20, 0, tex_grass);
	RenderWalls(1000, 20, 0, tex_grass);
	RenderWalls(1050, 20, 0, tex_grass);
	glPopMatrix();

	// Draw right wall
	glPushMatrix();
	glTranslated(0, 0, 6);
	RenderWallsHelper(87, 30, tex_wall);
	RenderWalls(87, 20, 30, tex_wall);
	RenderWalls(137, 20, 30, tex_wall);
	RenderWalls(187, 20, 30, tex_wall);
	RenderWalls(237, 20, 30, tex_wall);
	RenderWallsHelper(520, 30, tex_cave);
	RenderWalls(520, 20, 30, tex_cave);
	RenderWalls(570, 20, 30, tex_cave);
	RenderWalls(620, 20, 30, tex_cave);
	RenderWalls(670, 20, 30, tex_cave);
	RenderWallsHelper(900, 30, tex_grass);
	RenderWalls(900, 20, 30, tex_grass);
	RenderWalls(950, 20, 30, tex_grass);
	RenderWalls(1000, 20, 30, tex_grass);
	RenderWalls(1050, 20, 30, tex_grass);
	glPopMatrix();

}

void wallpapers() {
	// Draw wallpaper
	glPushMatrix();
	glTranslated(0, 0, 6);
	renderPoster(100, 7.5, 12.5, 29.9, tex_poster);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, -6);
	renderPoster(120, 7.5, 12.5, 0.1, tex_poster2);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 6);
	renderPoster(140, 7.5, 12.5, 29.9, tex_poster_batman);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, -6);
	renderPoster(160, 7.5, 12.5, 0.1, tex_poster);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 6);
	renderPoster(180, 7.5, 12.5, 29.9, tex_poster2_batman);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, -6);
	renderPoster(200, 7.5, 12.5, 0.1, tex_poster2);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 6);
	renderPoster(220, 7.5, 12.5, 29.9, tex_poster);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, -6);
	renderPoster(240, 7.5, 12.5, 0.1, tex_poster_batman);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 6);
	renderPoster(260, 7.5, 12.5, 29.9, tex_poster2_batman);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, -6);
	renderPoster(280, 7.5, 12.5, 0.1, tex_poster2);
	glPopMatrix();
}

void buildings() {
	// render buildings on the right
	glPushMatrix();
	glTranslated(10, 10, 42);
	glScaled(6, 11, 4);
	drawBox(tex_building3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(28.5, 7.5, 42);
	glScaled(8, 8, 4);
	drawBox(tex_building5);
	glPopMatrix();

	glPushMatrix();
	glTranslated(65, 6, 42);
	glScaled(20, 8, 4);
	drawBox(tex_building4);
	glPopMatrix();

	glPushMatrix();
	glTranslated(300, 6, 42);
	glScaled(8, 15, 4);
	drawBox(tex_building);
	glPopMatrix();

	glPushMatrix();
	glTranslated(320, 6, 42);
	glScaled(8, 10, 4);
	drawBox(tex_building7);
	glPopMatrix();

	glPushMatrix();
	glTranslated(340, 6, 42);
	glScaled(8, 10, 4);
	drawBox(tex_building2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(360, 6, 42);
	glScaled(8, 10, 4);
	drawBox(tex_building8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(380, 6, 42);
	glScaled(8, 10, 4);
	drawBox(tex_building7);
	glPopMatrix();

	glPushMatrix();
	glTranslated(400, 6, 42);
	glScaled(8, 10, 4);
	drawBox(tex_building);
	glPopMatrix();

	glPushMatrix();
	glTranslated(420, 6, 42);
	glScaled(8, 10, 4);
	drawBox(tex_building2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(440, 6, 42);
	glScaled(8, 10, 4);
	drawBox(tex_building8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(460, 6, 42);
	glScaled(8, 20, 4);
	drawBox(tex_building);
	glPopMatrix();

	glPushMatrix();
	glTranslated(480, 6, 42);
	glScaled(8, 8, 4);
	drawBox(tex_building7);
	glPopMatrix();

	glPushMatrix();
	glTranslated(500, 6, 42);
	glScaled(8, 12, 4);
	drawBox(tex_building2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(735, 6, 42);
	glScaled(8, 12, 4);
	drawBox(tex_building);
	glPopMatrix();

	glPushMatrix();
	glTranslated(755, 5, 42);
	glScaled(8, 12, 4);
	drawBox(tex_building8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(775, 6, 42);
	glScaled(8, 12, 4);
	drawBox(tex_building2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(795, 10, 42);
	glScaled(8, 12, 4);
	drawBox(tex_building7);
	glPopMatrix();

	glPushMatrix();
	glTranslated(815, 10, 42);
	glScaled(8, 12, 4);
	drawBox(tex_building8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(835, 10, 42);
	glScaled(8, 12, 4);
	drawBox(tex_building2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(855, 10, 42);
	glScaled(8, 12, 4);
	drawBox(tex_building);
	glPopMatrix();

	glPushMatrix();
	glTranslated(875, 10, 42);
	glScaled(8, 12, 4);
	drawBox(tex_building7);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1110, 10, 42);
	glScaled(6, 11, 4);
	drawBox(tex_building3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1128.5, 7.5, 42);
	glScaled(8, 8, 4);
	drawBox(tex_building5);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1160, 6, 42);
	glScaled(20, 8, 4);
	drawBox(tex_building4);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1200, 7.5, 42);
	glScaled(15, 8, 4);
	drawBox(tex_building4);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1230, 6, 42);
	glScaled(12, 12, 4);
	drawBox(tex_building6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1255, 10, 42);
	glScaled(8, 11, 4);
	drawBox(tex_building3);
	glPopMatrix();

	// render buildings on the left
	glPushMatrix();
	glTranslated(20, 7.5, -12);
	glScaled(15, 8, 4);
	drawBox(tex_building4);
	glPopMatrix();

	glPushMatrix();
	glTranslated(50, 6, -12);
	glScaled(12, 12, 4);
	drawBox(tex_building6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(77, 10, -12);
	glScaled(8, 11, 4);
	drawBox(tex_building3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(320, 12, -18);
	glScaled(8, 11, 10);
	drawBox(tex_building2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(340, 10, -12);
	glScaled(8, 11, 4);
	drawBox(tex_building8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(360, 10, -12);
	glScaled(8, 11, 4);
	drawBox(tex_building);
	glPopMatrix();

	glPushMatrix();
	glTranslated(380, 10, -12);
	glScaled(8, 11, 4);
	drawBox(tex_building7);
	glPopMatrix();

	glPushMatrix();
	glTranslated(400, 10, -12);
	glScaled(8, 11, 4);
	drawBox(tex_building);
	glPopMatrix();

	glPushMatrix();
	glTranslated(420, 10, -12);
	glScaled(8, 11, 4);
	drawBox(tex_building8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(440, 10, -12);
	glScaled(8, 11, 4);
	drawBox(tex_building2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(460, 10, -12);
	glScaled(8, 20, 4);
	drawBox(tex_building7);
	glPopMatrix();

	glPushMatrix();
	glTranslated(480, 10, -12);
	glScaled(8, 11, 4);
	drawBox(tex_building);
	glPopMatrix();

	glPushMatrix();
	glTranslated(500, 10, -12);
	glScaled(8, 20, 4);
	drawBox(tex_building8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(735, 10, -18);
	glScaled(8, 20, 10);
	drawBox(tex_building2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(755, 10, -12);
	glScaled(8, 15, 4);
	drawBox(tex_building7);
	glPopMatrix();

	glPushMatrix();
	glTranslated(775, 10, -12);
	glScaled(8, 10, 4);
	drawBox(tex_building8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(795, 15, -12);
	glScaled(8, 20, 4);
	drawBox(tex_building);
	glPopMatrix();

	glPushMatrix();
	glTranslated(815, 15, -12);
	glScaled(8, 15, 4);
	drawBox(tex_building7);
	glPopMatrix();

	glPushMatrix();
	glTranslated(835, 15, -12);
	glScaled(8, 15, 4);
	drawBox(tex_building2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(855, 15, -12);
	glScaled(8, 15, 4);
	drawBox(tex_building8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(875, 15, -12);
	glScaled(8, 20, 4);
	drawBox(tex_building);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1120, 7.5, -12);
	glScaled(15, 8, 4);
	drawBox(tex_building4);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1150, 6, -12);
	glScaled(12, 12, 4);
	drawBox(tex_building6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1177, 10, -12);
	glScaled(8, 11, 4);
	drawBox(tex_building3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1195, 10, -12);
	glScaled(6, 11, 4);
	drawBox(tex_building3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1210, 7.5, -12);
	glScaled(8, 8, 4);
	drawBox(tex_building5);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1240, 6, -12);
	glScaled(20, 8, 4);
	drawBox(tex_building4);
	glPopMatrix();

}

void renderServiceLane(float x1, float x2, float z1, float z2, GLTexture tex) {
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(x1, 0, z1);
	glTexCoord2f(1, 0);
	glVertex3f(x2, 0, z1);
	glTexCoord2f(1, 1);
	glVertex3f(x2, 0, z2);
	glTexCoord2f(0, 1);
	glVertex3f(x1, 0, z2);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void serviceLane() {
	renderServiceLane(0, 100, -3, 0, tex_street);
	renderServiceLane(0, 100, 30, 33, tex_street);
	renderServiceLane(100, 200, -3, 0, tex_street);
	renderServiceLane(100, 200, 30, 33, tex_street);
	renderServiceLane(200, 300, -3, 0, tex_street);
	renderServiceLane(200, 300, 30, 33, tex_street);
	renderServiceLane(300, 400, -3, 0, tex_street);
	renderServiceLane(300, 400, 30, 33, tex_street);
	renderServiceLane(400, 500, -3, 0, tex_street);
	renderServiceLane(400, 500, 30, 33, tex_street);
	renderServiceLane(500, 600, -3, 0, tex_street);
	renderServiceLane(500, 600, 30, 33, tex_street);
	renderServiceLane(600, 700, -3, 0, tex_street);
	renderServiceLane(600, 700, 30, 33, tex_street);
	renderServiceLane(700, 800, -3, 0, tex_street);
	renderServiceLane(700, 800, 30, 33, tex_street);
	renderServiceLane(800, 900, -3, 0, tex_street);
	renderServiceLane(800, 900, 30, 33, tex_street);
	renderServiceLane(900, 1000, -3, 0, tex_street);
	renderServiceLane(900, 1000, 30, 33, tex_street);
	renderServiceLane(1000, 1100, -3, 0, tex_street);
	renderServiceLane(1000, 1100, 30, 33, tex_street);
	renderServiceLane(1100, 1200, -3, 0, tex_street);
	renderServiceLane(1100, 1200, 30, 33, tex_street);
	renderServiceLane(1200, 1300, -3, 0, tex_street);
	renderServiceLane(1200, 1300, 30, 33, tex_street);
}

void restartGame() {
	time = 0;


	x_rpg = 27;
	bullet = false;
	rpg_range = 0;
	out = false;

	bullets.clear();
	gameover = false;

	rot_angle_joker_left = 0;

	shoot2 = false;
	rot_angle_joker_car_left = 0;

	escaped = false;
	escape_counter = 0;

	rot_angle_joker = 0;
	rot_angle_joker_car_right = 0;
	x_joker_factor = 0;

	dir = -1;
	lane = 3;
	predicted_lane;
	batman_lane = 3;
	counter = 1;

	batmobile_length = 11;
	joker_car_length = 10;

	hit = false;
	shoot = false;
	hit_range = 0;

	x_joker = 28;
	z_joker = 15;

	x_joker_car = 30;

	jLeft = false;
	jRight = false;

	x_step = 0;
	z_step = 15;
	x_camera = -20.0f;

	move_right = false;
	move_left = false;
	paused = false;

	PlaySound("sounds/batman_theme.wav", NULL, SND_ASYNC || SND_FILENAME);
	glutIdleFunc(Anim);
}
//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{
	/*GLfloat fogColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 1.0f);
	glHint(GL_FOG_HINT, GL_DONT_CARE); glFogf(GL_FOG_START, 50.0f); // Fog Start Depth 
	glFogf(GL_FOG_END, 2000.0f); // Fog End Depth
	glEnable(GL_FOG);*/

	if (counter % 250 == 0 && !hit && !shoot && !escaped && !shoot2) {
		if (!jRight && !jLeft) {
			float z_bullet;
			switch (lane) {
			case 1: z_bullet = 4.5; break;
			case 2: z_bullet = 10; break;
			case 3: z_bullet = 15; break;
			case 4: z_bullet = 20; break;
			case 5: z_bullet = 25.5; break;
			}
			bullet = true;
			rpg_range = 0;
			bullets.push_back(Bullets(x_joker + 2.5, z_bullet, lane)); //new: adding bullets
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	printf("bamtan: %i, joker %i \n", batman_lane, lane);
	if (escaped) {
		if (escape_counter == 1) {
			PlaySound("sounds/laugh2.wav", NULL, SND_ASYNC || SND_FILENAME);
		}
		if (escape_counter >= 300) {
			glutIdleFunc(NULL);
		}
	}

	if (time >= 45) {
		escaped = true;
	}

	if (counter % 170 == 0) {
		jChange();
	}
	

	if (counter % 140 == 0) {
		time += 1;
	}

	if (move_right && hit) {
		// same scenario as shooting the car
		shoot = true;
		move_right = false;
		hit = false;
		PlaySound("sounds/car_skid.wav", NULL, SND_ASYNC | SND_FILENAME);
	}
	else {
		if (move_left && hit) {
			// same as above but the rotation is in the other direction
			shoot2 = true;
			move_left = false;
			hit = false;
			PlaySound("sounds/car_skid.wav", NULL, SND_ASYNC | SND_FILENAME);
		}
	}

	char score_text[100] = "Time left: ";
	char timer[100];
	if (escaped) {
		sprintf(timer, "%d", 0);
	}
	else {
		sprintf(timer, "%d", 45 - time);
	}
	strcat(score_text, timer);
	print(score_text, 14 + x_camera, 18, 6.5);

	//printf("Time: %i \n", 45 - time);


	if (batman_lane == lane && x_step + (batmobile_length / 2) - 5.6 > x_joker - (joker_car_length / 2) && !hit) {
		hit_range = 10;
		hit = true;
	}

	// importing sound
	if (counter % 5 == 0 && counter < 10) {
		PlaySound("sounds/batman_theme.wav", NULL, SND_ASYNC || SND_FILENAME);
	}

	glLoadIdentity();
	gluLookAt(x_camera, 20.0f, 15.0f, 20.0f + x_camera, 10.0f, 15.0f, 0.0f, 1.0f, 0.0f);
		

	// draw bullets
	DrawBullets(); //new

	// render service lane
	serviceLane();

	// render sidewalk
	sidewalk();
		
	// render walls
	walls();

	// render wallpapers
	wallpapers();

	// render buildings
	buildings();
		
	// render ground
	RenderGround();

	// Draw joker model
	glPushMatrix();
		
	glPushMatrix();
	glColor3f(0.54, 0.168, 0.886);
	if (shoot || shoot2) {
		glTranslatef(x_joker + x_joker_factor, 4, z_joker);
	}
	else {
		glTranslatef(x_joker, 4, z_joker);
	}
	glRotated(90, 0, 0, 1);
	glRotated(270, 0, 1, 0);
	glScaled(3, 3, 3);
	if (shoot) {
		if (rot_angle_joker == -180) {
			glutIdleFunc(NULL);
		}
		glRotated(rot_angle_joker, 0, 0, 1);
	}
	else {
		if (shoot2) {
			if (rot_angle_joker_left == 180) {
				PlaySound("sounds/smile.wav", NULL, SND_ASYNC | SND_FILENAME);
				glutIdleFunc(NULL);
			}
			glRotated(rot_angle_joker_left, 0, 0, 1);
		}
	}
	model_joker.Draw();
	glPopMatrix();

	// Draw joker's car model
	glPushMatrix();
	glPushMatrix();
	glColor3f(0.0, 1.0, 0.0);
	glTranslatef(x_joker_car, 2.3, z_joker);
	glRotated(90, 0, 1, 0);
	glScalef(0.028, 0.028, 0.022);
	if (shoot) {
		if (rot_angle_joker_car_right == 180) {
			PlaySound("sounds/smile.wav", NULL, SND_ASYNC | SND_FILENAME);
			glutIdleFunc(NULL);
		}
		glRotated(rot_angle_joker_car_right, 0, 1, 0);
	}
	else {
		if (shoot2) {
			if (rot_angle_joker_car_left == -180) {
				PlaySound("sounds/smile.wav", NULL, SND_ASYNC | SND_FILENAME);
				glutIdleFunc(NULL);
			}
			glRotated(rot_angle_joker_car_left, 0, 1, 0);
		}
	}
	
	model_joker_car.Draw();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();

	if (!shoot && !escaped && !shoot2 && !hit) {
		glPushMatrix();
		if (bullet) {
			glTranslated(-1 * rpg_range, 0, 0);
		}
		glTranslated(x_rpg, 4, z_joker); // cylinder at (0,0,1)
		glRotated(90, 0, 1, 0);
		GLUquadricObj * qobj;
		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_LINE);
		gluCylinder(qobj, 0.5, 0.5, 3.0, 100, 100);
		glPopMatrix();
	}
	
	// Draw Batmobile
	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslated(x_step, -0.5, z_step);
	glRotatef(90.f, 0, 1, 0);
	glScaled(0.07, 0.08, 0.08);
	model_batmobile.Draw();
	glPopMatrix();


	glutSwapBuffers();
}

//=======================================================================
// Keyboard Function
//=======================================================================
void key(unsigned char key, int x, int y)
{
	if (key == 'r') {
		restartGame();
	}

	if (key == 'p' && paused) {
		glutIdleFunc(Anim);
		paused = false;
	}
	else {
		if (key == 'p' && !paused) {
			glutIdleFunc(nullptr);
			paused = true;
		}
	}
	if (key == 'd' && !move_left && !move_right && z_step <= 21) {
		move_right = true;
		z_end_right = z_step + 6;
		batman_lane++;
	}
	else {
		if (key == 'a' && !move_right && !move_left && z_step >= 9) {
			move_left = true;
			z_end_left = z_step - 6;
			batman_lane--;
		}
	}
}

//=======================================================================
// Mouse Function
//=======================================================================
/*void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
}*/

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_batmobile.Load("Models/batmobile/batmobile.3ds");
	model_joker_car.Load("Models/car2/car2.3ds");
	model_joker.Load("Models/joker/joker.3ds");

	// Loading texture files
	tex_ground.Load("Textures/road.bmp");
	tex_service_lane.Load("Textures/service.bmp");
	tex_wall.Load("Textures/bricks.bmp");
	tex_cave.Load("Textures/cave.bmp");
	tex_grass.Load("Textures/grass.bmp");
	tex_poster.Load("Textures/wallpaper.bmp");
	tex_poster2.Load("Textures/wallpaper2.bmp");
	tex_sidewalk.Load("Textures/sidewalk.bmp");
	tex_street.Load("Textures/street.bmp");
	tex_building.Load("Textures/building.bmp");
	tex_building2.Load("Textures/building2.bmp");
	tex_building3.Load("Textures/building3.bmp");
	tex_building4.Load("Textures/building4.bmp");
	tex_building5.Load("Textures/building5.bmp");
	tex_building6.Load("Textures/building6.bmp");
	tex_building7.Load("Textures/building7.bmp");
	tex_building8.Load("Textures/building8.bmp");
	tex_poster_batman.Load("Textures/batman_poster.bmp");
	tex_poster2_batman.Load("Textures/batman_poster2.bmp");
}


void myMouse(int button, int state, int x, int y)
{

	if (state == GLUT_DOWN)
	{
		if (batman_lane == lane && x_step + (batmobile_length/2) - 1 >= x_joker - (joker_car_length/2))
		{
			shoot = true;
			PlaySound("sounds/car_skid.wav", NULL, SND_ASYNC | SND_FILENAME);
		}
	}
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(key);

	glutIdleFunc(Anim);

	//glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	//glutReshapeFunc(myReshape);

	myInit();

	LoadAssets();

	glutMainLoop();
}
