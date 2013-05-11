#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gl/GL.h>
#include <gl/GLUT.h>
#include <math.h>

#include "BSPTree.h"
#include "PointLightSource.h"

#define WIDTH 800
#define HEIGHT 600

#define XSENSITIVITY 0.01
#define YSENSITIVITY 0.01

static double distance = 10, direction, pitch;
static double target[3] = { 0, 0, 0 };
static int lastx,lasty;
static int buttonPressed;
static PointLightSource* lightSource;
static BSPTree* tree;

void Keyboard(unsigned char key, int x, int y)
{
	if (key == 'q')
		exit(0);
}
void Motion(int x, int y)
{
	if (buttonPressed == 0)
	{
		direction -= (x - lastx)*XSENSITIVITY;
		while (direction > 2*M_PI)
			direction -= 2*M_PI;
		while (direction < 0)
			direction += 2*M_PI;
		pitch += (y - lasty)*YSENSITIVITY;
		if (pitch > M_PI/2)
			pitch = M_PI/2;
		if (pitch < -M_PI/2)
			pitch = -M_PI/2;
	}
	lastx = x;
	lasty = y;
}
void Mouse(int button, int state, int x, int y)
{
	buttonPressed = button;
	lastx = x;
	lasty = y;
}

void DrawGraphics (void)
{
	double position[3];
	position[0] = sin(direction)*cos(pitch)*distance;
	position[1] = sin(pitch)*distance;
	position[2] = cos(direction)*cos(pitch)*distance;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(position[0],position[1],position[2],target[0],target[1],target[2],0,1,0);
	glColor3f(1,0,0);
	glBegin(GL_QUADS);
	glNormal3f(0,0,1); glVertex3d(-1,-1,1); glVertex3d(1,-1,1); glVertex3d(1,1,1); glVertex3d(-1,1,1);
	glNormal3f(1,0,0); glVertex3d(1,-1,1); glVertex3d(1,-1,-1); glVertex3d(1,1,-1); glVertex3d(1,1,1);
	glNormal3f(0,0,-1); glVertex3d(1,-1,-1); glVertex3d(-1,-1,-1); glVertex3d(-1,1,-1); glVertex3d(1,1,-1);
	glNormal3f(-1,0,0); glVertex3d(-1,-1,-1); glVertex3d(-1,-1,1); glVertex3d(-1,1,1); glVertex3d(-1,1,-1);
	glNormal3f(0,-1,0); glVertex3d(-1,-1,1); glVertex3d(-1,-1,-1); glVertex3d(1,-1,-1); glVertex3d(1,-1,1);
	glNormal3f(0,1,0); glVertex3d(-1,1,1); glVertex3d(1,1,1); glVertex3d(1,1,-1); glVertex3d(-1,1,-1);
	glEnd();
	glutSwapBuffers();
}

int main (int argc, char* argv[])
{
	float light[4] = {10,10,10,1};
	lightSource = new PointLightSource(Point(light[0],light[1],light[2]),Vector3d(0,0,0),Vector3d(1,1,1));
	tree = new BSPTree();
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(WIDTH,HEIGHT);
	glutCreateWindow("Shadow BSP");
	glutDisplayFunc(DrawGraphics);
	glutIdleFunc(DrawGraphics);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	
	glClearColor(0,0,0,1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,WIDTH/HEIGHT,1,100);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light);
	
	glutMainLoop();
	return 0;
}