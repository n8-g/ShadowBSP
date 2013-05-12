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
		
void PointLightSource::determineShadow(vector<Polygon>& lit, vector<Polygon>& shadowed, BSPNode* shadowNode, Polygon& fragment)
{
	if (shadowNode->is_leaf())
	{
		if (shadowNode->out())
		{
			//printf("Lighting fragment\n");
			//fragment.add_light(index); // Add the GL light index to the fragment's light list
			for (int i = 0; i < fragment.size(); ++i)
				shadowNode->add_polygon(Polygon(position,fragment[i],fragment[(i+1)%fragment.size()]));
			lit.push_back(fragment);
		}
		else
			shadowed.push_back(fragment);
		//treeNode->fragments().push_back(fragment);
	}
	else
	{
		//printf("Splitting fragment\n");
		Plane plane = shadowNode->plane();
		Polygon front,back;
		fragment.split(front,back,plane.n,plane.d); // Split by the shadow plane
		//printf("%d front points, %d back points\n",front.size(),back.size());
		if (front.size() > 2)
			determineShadow(lit,shadowed,shadowNode->front(),front);
		if (back.size() > 2)
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