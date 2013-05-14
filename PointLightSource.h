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

class PointLightSource
{
    public:
        // - constructors
        PointLightSource(const Point &position, const Vector3d &direction,const Vector3d &color, int index);
    
        PointLightSource();
		
		void determineShadow(std::vector<Polygon>& lit, std::vector<Polygon>& shadowed, BSPNode* shadowNode, Polygon& polygon);
    
        // Position of the light source
        Point position;
    
        // Direction of the light source
        // Added it just in case we want directional light source
        Vector3d direction;
        // Color of the light source
        Vector3d color;
		
		// GL light index
		int index;
};



#endif /* defined(CGEO_PROJECT_2_POINTLIGHTSOURCE_H) */
