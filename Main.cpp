#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gl/GL.h>
#include <gl/GLUT.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>

#include "BSPNode.h"
#include "PointLightSource.h"
#include "Utility.h"

#define WIDTH 800
#define HEIGHT 600

#define XSENSITIVITY 0.01
#define YSENSITIVITY 0.01

#define MOVESPEED 1 // 1 unit per second

using namespace std;

static double dist = 5, direction, pitch = M_PI/4;
static double target[3] = { 0, 0, 0 };
static double position[3];
static int lastx,lasty;
static int buttonPressed;
static vector<PointLightSource> lightSources;
static BSPNode tree;
static BSPNode shadowTree;
static bool enableLighting = true;
static bool keystate[256];
static int lightSource = 0;
static double elapsed;
static bool enableLines = true;
static bool frontToBack = false;
static bool singleLight = false;

int UpdateLighting(void);

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
	case 'o':
		enableLines = !enableLines;
		break;
	case 'l':
		enableLighting = !enableLighting;
		break;
	case 'f':
		frontToBack = !frontToBack;
		break;
	case 'y':
		singleLight = !singleLight;
		UpdateLighting();
		break;
	case '`':
		lightSource = -1;
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '0':
		lightSource = key - '0';
		if (singleLight)
			UpdateLighting();
		break;
	}
	keystate[key] = true;
}

void KeyboardUp(unsigned char key, int x, int y)
{
	keystate[key] = false;
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

bool DrawFragments (BSPNode* node, void* data)
{
	for (int i = 0; i < node->fragments().size(); ++i)
	{
		Polygon& fragment = node->fragments()[i];
		glPushAttrib(GL_LIGHTING_BIT);
		for (int l = 0; l < lightSources.size(); ++l)
			if (fragment.has_light(l))
				glEnable(GL_LIGHT0+l);
		glBegin(GL_POLYGON);
		glColor3f(1,1,1);
		glNormal3f(fragment.normal().x,fragment.normal().y,fragment.normal().z);
		for (int i = 0; i < fragment.size(); ++i)
			glVertex3f(fragment[i].x,fragment[i].y,fragment[i].z);
		glEnd();
		if (enableLines)
		{
			glColor3f(0,0,0);
			glBegin(GL_LINE_LOOP);
			for (int i = 0; i < fragment.size(); ++i)
				glVertex3f(fragment[i].x,fragment[i].y,fragment[i].z);
			glEnd();
		}
		glPopAttrib();
	}
}

bool GenerateShadows(BSPNode* node, void* data)
{
    PointLightSource *pls = (PointLightSource*) data;
	vector<Polygon> fragments = node->fragments(); // Use the last list of fragments so we can have multiple light sources
	node->fragments().clear(); // Clear the node's list of fragments since we're going to regenerate it
	for (int i = 0; i < fragments.size(); ++i)
	{
	    Polygon& p = fragments[i];
		Vector3d normal = p.normal();
		//cout << "Generating shadows for " << p << endl;
		float dot = normal.dot(pls->position-p[0]);
	    if(dot > 0) // Facing the light
			pls->determineShadow(node,&shadowTree,p);
		else node->fragments().push_back(p); // Keep it in the tree in case another light illuminates it
	}
	return true;
}

int UpdateLighting(void)
{
	//cout << "Updating lighting" << endl;
	tree.init_fragments();
    for (int i = 0; i < lightSources.size(); i++) {
		if (singleLight && i != lightSource)
			continue;
		shadowTree.clear();
		PointLightSource& light = lightSources[i];
	    tree.traverse(true, lightSources[i].position, GenerateShadows, &lightSources[i]);
	}
}

void DrawGraphics (void)
{
	int time = glutGet(GLUT_ELAPSED_TIME);
	double delta = (time - elapsed)/1000.0f;
	double move_vector[3] = { 0, 0, 0 };
	elapsed = time;
	if (keystate['-']) dist += delta * MOVESPEED;
	else if (keystate['=']) dist -= delta * MOVESPEED;
	position[0] = sin(direction)*cos(pitch)*dist;
	position[1] = sin(pitch)*dist;
	position[2] = cos(direction)*cos(pitch)*dist;
    
	if (keystate['w']) move_vector[2] = -1;
	else if (keystate['s']) move_vector[2] = 1;
	if (keystate['a']) move_vector[0] = -1;
	else if (keystate['d']) move_vector[0] = 1;
	if (keystate['q']) move_vector[1] = -1;
	else if (keystate['e']) move_vector[1] = 1;

	if (move_vector[0] || move_vector[1] || move_vector[2])
	{
		if (lightSource < 0)
		{
			target[0] += move_vector[0] * MOVESPEED * delta;
			target[1] += move_vector[1] * MOVESPEED * delta;
			target[2] += move_vector[2] * MOVESPEED * delta;
		}
		else if (lightSource < lightSources.size())
		{
			lightSources[lightSource].position.x += move_vector[0] * MOVESPEED * delta;
			lightSources[lightSource].position.y += move_vector[1] * MOVESPEED * delta;
			lightSources[lightSource].position.z += move_vector[2] * MOVESPEED * delta;
			UpdateLighting();
		}
	}
	
    //for(int i=0; i<lightSources.size(); i++)
    //    lightSources[i].shadowTree.traverse(Point(position[0],position[1],position[2]), traverse_test, NULL);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(position[0],position[1],position[2],target[0],target[1],target[2],0,1,0);
	glDisable(GL_LIGHTING);
    for (int i = 0; i < lightSources.size(); i++) {
		PointLightSource& light = lightSources[i];
		float lp[4] = { 0, 0, 0, 1 };
		float lc[4] = { 1, 1, 1, 1 };
		lp[0] = light.position.x;
		lp[1] = light.position.y;
		lp[2] = light.position.z;
		lc[0] = light.color.x;
		lc[1] = light.color.y;
		lc[2] = light.color.z;
		glLightfv(GL_LIGHT0+light.index, GL_POSITION, lp);
		glLightfv(GL_LIGHT0+light.index, GL_DIFFUSE, lc);
		glColor3fv(lc);
		glBegin(GL_POINTS);
		glVertex3fv(lp);
		glEnd();
	}
	if (enableLighting)
		glEnable(GL_LIGHTING);
	glColor3f(1,0,0);
	tree.traverse(frontToBack,Point(position[0],position[1],position[2]),DrawFragments,NULL);
	//glBegin(GL_QUADS);
	//glNormal3f(0,0,1); glVertex3d(-1,-1,1); glVertex3d(1,-1,1); glVertex3d(1,1,1); glVertex3d(-1,1,1);
	//glNormal3f(1,0,0); glVertex3d(1,-1,1); glVertex3d(1,-1,-1); glVertex3d(1,1,-1); glVertex3d(1,1,1);
	//glNormal3f(0,0,-1); glVertex3d(1,-1,-1); glVertex3d(-1,-1,-1); glVertex3d(-1,1,-1); glVertex3d(1,1,-1);
	//glNormal3f(-1,0,0); glVertex3d(-1,-1,-1); glVertex3d(-1,-1,1); glVertex3d(-1,1,1); glVertex3d(-1,1,-1);
	//glNormal3f(0,-1,0); glVertex3d(-1,-1,1); glVertex3d(-1,-1,-1); glVertex3d(1,-1,-1); glVertex3d(1,-1,1);
	//glNormal3f(0,1,0); glVertex3d(-1,1,1); glVertex3d(1,1,1); glVertex3d(1,1,-1); glVertex3d(-1,1,-1);
	//glEnd();
	glutSwapBuffers();
}

vector<Polygon> ParseOBJ(const char* objFile)
{
	double x, y, z;
	vector<Point> points;
	vector<Polygon> polygons;
	vector<Point> polypoints;
	char buffer[256];
	char* ptr,*end;
	int index;
	FILE* file = fopen(objFile,"r");
	
	while (fgets(buffer,256,file))
	{
		switch (*buffer)
		{
		case 'v':
			if (buffer[1] == 'n') continue; // ignore normals
			x = strtod(buffer+1,&ptr);
			y = strtod(ptr,&ptr);
			z = strtod(ptr,&ptr);
			printf("v %f %f %f\n",x,y,z);
			points.push_back(Point(x,y,z));
			break;
		case 'f':
			polypoints.clear();
			ptr = buffer+1;
			printf("f ");
			while(true)
			{
				index = strtoul(ptr,&end,0);
				if (!index) break; // If nothing was parsed, break
				printf("%d ",index);
				ptr = end;
				while(*ptr && !isspace(*ptr)) ++ptr; // Skip to next vertex, ignoring texture and normals
				polypoints.push_back(points[index-1]);
			}
			printf("\n");
			polygons.push_back(polypoints);
			break;
		}
	}
	return polygons;
}


int LoadScene(const char* sceneFile)
{
	vector<Polygon> polygons;
	vector<Point> points;
    
	ifstream fin(sceneFile);
    
	// number of points
	int n = 0;
	fin >> n;
	
	// read in points
	for (int i = 0; i < n; i++) {
		float x, y ,z;
		fin >> x >> y >> z;
		
		points.push_back(Point(x, y, z));
	}
    
	// number of polygons
	int m = 0;
	fin >> m;
    
	// read in indices and create polygons
	for (int i = 0; i < m; i++) {
		int id1, id2, id3;
		fin >> id1 >> id2 >> id3;
		Polygon p(points[id1], points[id2], points[id3]);
		polygons.push_back(p);
	}

	Utility::permute_list(polygons);
    
	tree.add_polygons(polygons);
	tree.dump();

    //tree.traverse(viewPoint, traverse_test, NULL);
    //cout<<"!!!!!!!!!\n\n";
    
    // number of light sources
	int l = 0;
	fin >> l;
    
    for (int i = 0; i < l; i++) {
		float lp[4] = { 0, 0, 0, 1 };
        float r, g, b;
		fin >> lp[0] >> lp[1] >> lp[2];
        fin >> r >> g >> b;
        lightSources.push_back(PointLightSource(Point(lp[0],lp[1],lp[2]), Vector3d(0,0,0), Vector3d(r,g,b),i));
    }
	
    
    fin.close();
	return 0;
}

int main (int argc, char* argv[])
{
	if (argc < 2)
	{
		fprintf(stderr,"usage: %s <model>\n",argv[0]);
		return -1;
	}
	
	LoadScene(argv[1]);
	if (argv[2])
	{
		vector<Polygon> polygons = ParseOBJ(argv[2]);
		Utility::permute_list(polygons);
		tree.add_polygons(polygons);
	}
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(WIDTH,HEIGHT);
	glutCreateWindow("Shadow BSP");
	glutDisplayFunc(DrawGraphics);
	glutIdleFunc(DrawGraphics);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	
	glClearColor(0,0,0,1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,WIDTH/HEIGHT,1,100);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glPointSize(10);
	//glEnable(GL_DEPTH_TEST);

	//tree.init_fragments();
	UpdateLighting();
	
	glutMainLoop();
	return 0;
}