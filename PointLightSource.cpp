//
//  PointLightSource.cpp
//  
//
//  Created by Aykut Simsek on 5/2/13.
//
//
#include <iostream>
#include "PointLightSource.h"
#include "Utility.h"


using namespace std;

Vector3d::Vector3d()
{
    
}

Vector3d::Vector3d(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3d::Vector3d(const Vector3d &copy)
{
	this->x = copy.x;
	this->y = copy.y;
	this->z = copy.z;
}


PointLightSource::PointLightSource(const Point &position, const Vector3d &direction, const Vector3d &color, int index):position(position),direction(0,0,0),color(color),index(index) {}
		
void PointLightSource::determineShadow(BSPNode* treeNode, BSPNode* shadowNode, Polygon& fragment)
{
	if (shadowNode->is_leaf())
	{
		if (shadowNode->out())
		{
			printf("Lighting fragment\n");
			fragment.add_light(index); // Add the GL light index to the fragment's light list
			shadowNode->add_polygon(Polygon(position,fragment[0],fragment[1]));
			shadowNode->add_polygon(Polygon(position,fragment[1],fragment[2]));
			shadowNode->add_polygon(Polygon(position,fragment[2],fragment[0]));
		}
		treeNode->fragments().push_back(fragment);
	}
	else
	{
		printf("Splitting fragment\n");
		Plane plane = shadowNode->plane();
		Polygon front,back;
		fragment.split(front,back,plane.n,plane.d); // Split by the shadow plane
		printf("%d front points, %d back points\n",front.size(),back.size());
		if (front.size())
			determineShadow(treeNode,shadowNode->front(),front);
		if (back.size())
			determineShadow(treeNode,shadowNode->back(),back);
	}
}

void print_point(const Point &p)
{
	cout << "(" << p.x << ", " << p.y << ", " << p.z << ")" << flush;
}

void print_polygon(const Polygon &p)
{
	print_point(p[0]);
	cout << "-";
	print_point(p[1]);
	cout << "-";
	print_point(p[2]);
}