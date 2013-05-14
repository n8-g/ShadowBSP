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

PointLightSource::PointLightSource(const Point &position, const Vector3d &direction, const Vector3d &color, int index):position(position),direction(0,0,0),color(color),index(index) {}
		
void PointLightSource::determineShadow(vector<Polygon>& lit, vector<Polygon>& shadowed, BSPNode* shadowNode, Polygon& polygon)
{
	if (shadowNode->is_leaf())
	{
		if (shadowNode->out())
		{
			int size = polygon.size();

			for (int i = 0; i < size; ++i)
				shadowNode->add_polygon(Polygon(position,polygon[i],polygon[(i+1)%size]));
			lit.push_back(polygon);
		}
		else
			shadowed.push_back(polygon);
		//treeNode->fragments().push_back(fragment);
	}
	else
	{
		//printf("Splitting fragment\n");
		const Plane& plane = shadowNode->plane();

		Polygon front,back;
		polygon.split(front,back,plane.n,plane.d); // Split by the shadow plane
		if (front.size())
			determineShadow(lit,shadowed,shadowNode->front(),front);
		if (back.size())
			determineShadow(lit,shadowed,shadowNode->back(),back);
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