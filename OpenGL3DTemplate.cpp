#include <glut.h>
#include <math.h>
#include <stdio.h>

bool descending = false;
float x_camera_start = -20.0f;
float x_camera_end = 20.0f;

float x_step_character = 1.0f;
float y_step_moving = 0.0f;
float z_step_moving = 0.0f;
float current_z_step = 7.5f;

bool moving_right = false;
float moving_right_span = 0.0f;
bool moving_left = false;
float moving_left_span = 0.0f;
bool moving_up = false;

float x_init_jump;

void character() {
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslated(1.2f, 1.0f, current_z_step);
	glutSolidCube(2);
	glPopMatrix();
}

void Display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(x_camera_start, 10.0f, 7.5f, x_camera_end, 0.0f, 7.5f, 0.0f, 1.0f, 0.0f);

	glPushMatrix();
	if (moving_right || moving_left) {
		glTranslated(x_step_character, 0.0f, z_step_moving);
	}
	else {
		if (moving_up) {
			glTranslated(x_step_character, y_step_moving, 0.0f);
		}
		else {
			glTranslated(x_step_character, 0.0f, 0.0f);
		}
	}
	character();
	glPopMatrix();

	// just for now that acts as a board, will be removed later.
	glPushMatrix();
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 15.0f);
	glVertex3f(200.0f, 0.0f, 15.0f);
	glVertex3f(200.0f, 0.0f, 0.0f);
	glEnd();
	glPopMatrix();

	printf("x: %f, y: %f \n", x_step_character, y_step_moving);

	glFlush();
}

void Anim() {
	x_step_character += 0.004;
	x_camera_start += 0.004;
	x_camera_end += 0.004;
	if (moving_right) {
		moving_right_span -= 0.01;
		z_step_moving = 3 - moving_right_span;
		if (moving_right_span <= 0) {
			moving_right = false;
			z_step_moving = 0.0f;
			current_z_step += 3;
		}
	}
	else {
		if (moving_left) {
			moving_left_span -= 0.01;
			z_step_moving = - (3 - moving_left_span);
			if (moving_left_span <= 0) {
				moving_left = false;
				z_step_moving = 0.0f;
				current_z_step -= 3;
			}
		}
		else {
			if (moving_up) {
				y_step_moving = sqrt(9 - ((x_step_character - x_init_jump - 3) * (x_step_character - x_init_jump - 3)));
				if (y_step_moving > 2.9) {
					descending = true;
				}
				if (y_step_moving <= 0.2 && descending) {
					moving_up = false;
					descending = false;
					y_step_moving = 0.0f;
				}
			}
		}
	}

	glutPostRedisplay();
}

void key(unsigned char key, int x, int y) {
	if (key == 'd' && !moving_left && !moving_right & current_z_step <= 10.5) {
		moving_right = true;
		moving_right_span = 3.0f;
	}
	if (key == 'a' && !moving_left && !moving_right && current_z_step >= 4.5) {
		moving_left = true;
		moving_left_span = 3.0f;
	}
	if (key == 'w') {
		moving_up = true;
		x_init_jump = x_step_character;
	}
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitWindowSize(1000, 700);
	glutInitWindowPosition(150, 150);

	glutCreateWindow("OpenGL - 3D Template");
	glutDisplayFunc(Display);
	glutIdleFunc(Anim);
	glutKeyboardFunc(key);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, 300 / 300, 0.1f, 300.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	

	glutMainLoop();
}
