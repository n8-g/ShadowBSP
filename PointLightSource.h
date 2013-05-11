//
//  PointLightSource.h
//  
//
//  Created by Aykut Simsek on 5/2/13.
//
//
#ifndef			CGEO_PROJECT_2_POINTLIGHTSOURCE_H
#define			CGEO_PROJECT_2_POINTLIGHTSOURCE_H

#include "Polygon.h"
#include "BSPTree.h"
#include "BSPNode.h"

struct Vector3d
{
	// the (x, y, z) coordinates
	float x;
	float y;
	float z;
    
	// - constructors
    Vector3d();
	// this one requires specific values for the coordiantes
	Vector3d(float x, float y, float z);
    
	// the copy constructor, which copys the valus from another Point object
	Vector3d(const Vector3d &copy);
};

class PointLightSource
{
    public:
        // - constructors
        PointLightSource(const Point &position, const Vector3d &direction,const Vector3d &color);
    
        PointLightSource();
    
        void shadowGenerator(BSPTree *bspTree);
    
        // Position of the light source
        Point position;
    
        // Direction of the light source
        // Added it just in case we want directional light source
        Vector3d direction;
        // Color of the light source
        Vector3d color;
        // Shadow Volume BSPTree for each light source
        BSPTree SVBSPtree;
};



#endif /* defined(CGEO_PROJECT_2_POINTLIGHTSOURCE_H) */
