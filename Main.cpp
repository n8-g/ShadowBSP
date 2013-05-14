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

static double dist = 5, direction, pitch = M_PI/4; // 'Eye' location
static double target[3] = { 0, 0, 0 }; // Target location
static double position[3]; // Caculated position of eye
static int lastx,lasty; // Last x and y for dragging the mouse
static int buttonPressed; // The current mouse button 
static vector<PointLightSource> lightSources; // A list of point light sources
static BSPNode tree; // The scene BSP tree
static BSPNode shadowTree; // The shadow BSP tree. Cleared for each light source
static bool enableLighting = true; // Enables GL lighting
static bool keystate[256]; // The keyboard state, for w,s,a,d,+, and -
static int lightSource = 0; // The current light source (the one to move and draw in single light mode) 
static double elapsed; // Time of last frame
static bool enableLines = true; // Enables outlines
static bool frontToBack = false; // Controls direction of scene BSP traversal
static bool singleLight = false; // Enables single light mode
static int nframes; // Number of frames in the last second
static int frame_timer; // 1 second timer for frame rate

void UpdateLighting(void); // Regenerates the shaded/lit fragments based on light position

void Keyboard(unsigned char key, int x, int y)
{
	// Keyboard controls
	switch (key)
	{
	case 27: // Escape
		exit(0);
	case 'o':
		enableLines = !enableLines;
		UpdateLighting();
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
	case 'u':
		UpdateLighting();
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
	// Store the key state for later
	keystate[key] = true;
}

void KeyboardUp(unsigned char key, int x, int y)
{
	// Reset the key state
	keystate[key] = false;
}
void Motion(int x, int y)
{
	if (buttonPressed == 0) // We're dragging
	{
		direction -= (x - lastx)*XSENSITIVITY; // Update direction
		while (direction > 2*M_PI) // Wrap direction
			direction -= 2*M_PI;
		while (direction < 0)
			direction += 2*M_PI;
		pitch += (y - lasty)*YSENSITIVITY; // Update pitch
		if (pitch > M_PI/2) // Clamp pitch
			pitch = M_PI/2;
		if (pitch < -M_PI/2)
			pitch = -M_PI/2;
	}
	lastx = x; // Update mouse position
	lasty = y;
}
void Mouse(int button, int state, int x, int y)
{
	buttonPressed = button; // Record current button
	lastx = x;
	lasty = y;
}

void DrawLightNode (LightNode* node)
{
	if (node->illuminated && node->shadowed) // Interior node
	{
		DrawLightNode(node->shadowed); // Draw the part of the tree not illuminated by this light
		glEnable(GL_LIGHT0+node->light); // Enable the light
		DrawLightNode(node->illuminated); // Draw the part of the tree illuminated by this light
		glDisable(GL_LIGHT0+node->light); // Disable the light
	}
	else
	{
		glBegin(GL_TRIANGLES); // Draw each polygon, tesselating them as we go. Since they're convex, we can connect any edge to the first vertex to produce a valid triangle
		for (int i = 0; i < node->fragments.size(); ++i)
		{
			Polygon& fragment = node->fragments[i];
			glColor3f(1,1,1);
			glNormal3f(fragment.normal().x,fragment.normal().y,fragment.normal().z);
			for (int i = 1; i < fragment.size()-1; ++i)
			{
				glVertex3f(fragment[0].x,fragment[0].y,fragment[0].z);
				glVertex3f(fragment[i].x,fragment[i].y,fragment[i].z);
				glVertex3f(fragment[i+1].x,fragment[i+1].y,fragment[i+1].z);
			}
		}
		glEnd();
		if (enableLines) // Draw the outlines if needed
		{
			glColor3f(0,0,0);
			for (int i = 0; i < node->fragments.size(); ++i)
			{
				Polygon& fragment = node->fragments[i];
				glBegin(GL_LINE_LOOP);
				for (int i = 0; i < fragment.size(); ++i)
					glVertex3f(fragment[i].x,fragment[i].y,fragment[i].z);
				glEnd();
			}
		}
	}
}

bool DrawFragments (BSPNode* node, void* data) // Drawing callback
{
	if (node->display_list != -1) // Good, we have a display list (cached commands). Call it
		glCallList(node->display_list);
	else
		DrawLightNode(&node->light_node()); // Draw directly
	return true;
}

bool GenerateShadows(LightNode* node, PointLightSource *pls)
{
	if (node->illuminated && node->shadowed) // We're an interior node. Generate shadows for children
	{
		GenerateShadows(node->illuminated,pls);
		GenerateShadows(node->shadowed,pls);
		return true;
	}
	// Leaf node - convert to interior node
	node->light = pls->index;
	node->illuminated = new LightNode();
	node->shadowed = new LightNode();
	//Utility::permute_list(node->fragments);
	for (int i = 0; i < node->fragments.size(); ++i)
	{
	    Polygon& p = node->fragments[i];
		Vector3d normal = p.normal();
		//cout << "Generating shadows for " << p << endl;
		double dot = normal.dot(pls->position-p[0]);
	    if(dot > 0) // Facing the light
			pls->determineShadow(node->illuminated->fragments,node->shadowed->fragments,&shadowTree,p);
		else node->shadowed->fragments.push_back(p); // Keep it in the tree in case another light illuminates it
	}
	node->fragments.clear(); // Our fragments are now in the leaves
	return true;
}

// Shadow generation callback
bool GenerateShadows(BSPNode* node, void* data)
{
	GenerateShadows(&node->light_node(),(PointLightSource*) data); 
	if (node->display_list == -1) // Create the display list if necessary
		node->display_list = glGenLists(1);
	// Compile a display list for rendering later. This way, GL can optomize it
	glNewList(node->display_list, GL_COMPILE);
	DrawLightNode(&node->light_node());
	glEndList();
	return true;
}

// Recalculates all shadows and as a bonus, updates a display list for drawing fragments in each BSP node
void UpdateLighting(void)
{
	int start = glutGet(GLUT_ELAPSED_TIME);
	//cout << "Updating lighting" << endl;
	tree.init_lighting();
    for (int i = 0; i < lightSources.size(); i++) {
		if (singleLight && i != lightSource)
			continue;
		PointLightSource& light = lightSources[i];
	    tree.traverse(true, lightSources[i].position, GenerateShadows, &lightSources[i]);
		shadowTree.clear();
	}
	printf("Lighting updated in %d milliseconds\n",glutGet(GLUT_ELAPSED_TIME)-start);
}
// Frame procedure
void DrawGraphics (void)
{
	int time = glutGet(GLUT_ELAPSED_TIME);
	double delta = (time - elapsed)/1000.0f;
	double move_vector[3] = { 0, 0, 0 };
	elapsed = time;
	++nframes;
	if (frame_timer < glutGet(GLUT_ELAPSED_TIME)) // Calculate fps
	{
		printf("%d frames per second\n",nframes);
		frame_timer = glutGet(GLUT_ELAPSED_TIME)+1000;
		nframes = 0;
	}
	// Time-based controls
	if (keystate['-']) dist += delta * MOVESPEED;
	else if (keystate['=']) dist -= delta * MOVESPEED;
	position[0] = sin(direction)*cos(pitch)*dist + target[0];
	position[1] = sin(pitch)*dist + target[1];
	position[2] = cos(direction)*cos(pitch)*dist + target[2];
    
	if (keystate['w']) move_vector[2] = -1;
	else if (keystate['s']) move_vector[2] = 1;
	if (keystate['a']) move_vector[0] = -1;
	else if (keystate['d']) move_vector[0] = 1;
	if (keystate['q']) move_vector[1] = -1;
	else if (keystate['e']) move_vector[1] = 1;

	if (move_vector[0] || move_vector[1] || move_vector[2]) // Move the light source or the target
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
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(position[0],position[1],position[2],target[0],target[1],target[2],0,1,0);
	glDisable(GL_LIGHTING);
    for (int i = 0; i < lightSources.size(); i++) { // Position and draw the lights
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
	if (enableLighting) // Enable lighting if needed
		glEnable(GL_LIGHTING);
	// Make everything white. The lights will color everything
	glColor3f(1,0,0);
	// Traverse the BSP tree in order and drawing the fragments in each
	tree.traverse(frontToBack,Point(position[0],position[1],position[2]),DrawFragments,NULL);
	// Present the new frame
	glutSwapBuffers();
}

// Parses an OBJ file (throwing away most of the gory details
bool ParseOBJ(vector<Polygon>& polygons, const char* objFile, float scale, float tx, float ty, float tz)
{
	double x, y, z;
	vector<Point> points;
	vector<Point> polypoints;
	char buffer[256];
	char* ptr,*end;
	int index;
	FILE* file = fopen(objFile,"r");
	if (!file)
	{
		fprintf(stderr,"Failed to open OBJ file '%s'\n",objFile);
		return false;
	}
	// Read a line
	while (fgets(buffer,256,file))
	{
		switch (*buffer)
		{
		case 'v': // Vertex definition
			if (buffer[1] != ' ') continue; // ignore normals and texcoords
			x = strtod(buffer+1,&ptr);
			y = strtod(ptr,&ptr);
			z = strtod(ptr,&ptr);
			
			printf("v %f %f %f\n",x,y,z);
			points.push_back(Point(x*scale+tx,y*scale+ty,z*scale+tz));
			break;
		case 'f': // Face definition
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
			polygons.push_back(Polygon(&polypoints[0],polypoints.size()));
			break;
		}
	}
	return true;
}

// Checks to see if a string starts with another string, and updates ptr to follow that other string if it does
inline bool starts_with(char* str, const char* search,char** ptr)
{
	int len = strlen(search);
	if (!strncmp(str,search,len))
	{
		if (ptr) *ptr = str + len;
		return true;
	}
	return false;
}

// Loads a scene from a file and adds it to the BSP tree
bool LoadScene(const char* sceneFile)
{
	char buffer[512];
	vector<Polygon> polygons;
	FILE* file = fopen(sceneFile,"r");
	if (!file)
	{
		fprintf(stderr,"Unable to open scene file '%s'\n",sceneFile);
		return false;
	}
	float tx=0,ty=0,tz=0,scale=1;
	while (fgets(buffer,512,file))
	{
		char* ptr = strchr(buffer,'\n');
		if (ptr) *ptr = 0; // Strip new line
		if (starts_with(buffer,"scale ",&ptr)) // Set scale
			scale = strtod(ptr,NULL);
		else if (starts_with(buffer,"translate ",&ptr)) // Set translate
		{
			tx = strtod(ptr,&ptr);
			ty = strtod(ptr,&ptr);
			tz = strtod(ptr,&ptr);
		}
		else if (starts_with(buffer,"obj ",&ptr)) // OBJ file reference
			ParseOBJ(polygons,ptr,scale,tx,ty,tz);
		else if (starts_with(buffer,"light ",&ptr)) // Light definition
		{
			float x,y,z,r,g,b;
			x = strtod(ptr,&ptr);
			y = strtod(ptr,&ptr);
			z = strtod(ptr,&ptr);
			r = strtod(ptr,&ptr);
			g = strtod(ptr,&ptr);
			b = strtod(ptr,&ptr);
	        lightSources.push_back(PointLightSource(Point(x,y,z), Vector3d(0,0,0), Vector3d(r,g,b),lightSources.size()));
		}
	}
	
	Utility::permute_list(polygons);
	tree.add_polygons(polygons); // Add all polygons to the BSP tree
	//tree.dump();

	return true;
}

int main (int argc, char* argv[])
{
	if (argc < 2)
	{
		fprintf(stderr,"usage: %s <model>\n",argv[0]);
		return -1;
	}
	
	// Load our scene
	if (!LoadScene(argv[1]))
		return -1;

	// Init GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(WIDTH,HEIGHT);
	glutCreateWindow("Shadow BSP");
	// Callbacks
	glutDisplayFunc(DrawGraphics);
	glutIdleFunc(DrawGraphics);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	
	// Init GL
	glClearColor(0,0,0,1); // Back color = black
	glMatrixMode(GL_PROJECTION); // Configure projection matrix
	glLoadIdentity();
	gluPerspective(45,WIDTH/HEIGHT,0.001,1000);
	glEnable(GL_CULL_FACE); // Don't draw the backs of faces
	glEnable(GL_LIGHTING); // Enable lighting calculations
	glEnable(GL_COLOR_MATERIAL); // Still use glColor to shade polygons
	glPointSize(20); // Big points for indicating light position
	//glEnable(GL_DEPTH_TEST); // NO DEPTH!!!

	tree.init_lighting();
	//UpdateLighting(); // Initial update of lighting
	
	glutMainLoop();
	return 0;
}