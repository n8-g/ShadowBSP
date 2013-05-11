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


PointLightSource::PointLightSource(const Point &position, const Vector3d &direction, const Vector3d &color):position(position),direction(0,0,0),color(color){}


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


void determine_shadow(BSPTree::CallbackContext *ctx)
{
    Polygon p = ctx->polygon;
    PointLightSource *pls = (PointLightSource*) ctx->user_data;
    if(Utility::coplanar_det(p,pls->position)>0)
    {
        (pls->SVBSPtree).add_polygon(p);
        for( int i=0; i<3 ; i++)
        {
            Polygon shadowPlane(pls->position,(p)[i],(p)[(i+1)%3]);
            (pls->SVBSPtree).add_polygon(shadowPlane);
        }
    }
    else
    {
        p.set_lit(false);
    }
}


void shadow_generator_loop(BSPTree::CallbackContext *ctx)
{
    Polygon p = ctx->polygon;
    PointLightSource *pls = (PointLightSource*) ctx->user_data;
    // We only care about the Polygons
    if (ctx->is_leaf) {
        return;
	}
 
    // if p is facing PLS
    if(Utility::coplanar_det(p, pls->position)>0){
        // ;; Determine areas of p that are shadowed. BSP node is p's node in BSPtree
        // SVBSPtree = determineShadow (p, SVBSPtree, PLS, BSPnode)
        (pls->SVBSPtree).filter_polygon(p, determine_shadow, pls);
    }
    //else
    else {
        // ;; p is not facing PLS or PLS is in p's plane
        // mark p as shadowed
        p.set_lit(false);
    }
    // endif
}


// ;; shadowGnerator determines shadow fragments that are attached to
// ;; appropriate node in the BSP tree for subsequent rendering.
// ;; Alternatively fragments could be written to a file.
void PointLightSource::shadowGenerator(BSPTree *bspTree){
    // ;; initialize the SVBSPtree to an OUT cell
    // SVBSPtree = OUT
    (this->SVBSPtree).clear();
    
    // ;; Process all polygons facing light source PLS in front-to-back
    // ;; order by BSP tree traversal in O(n).
    
    // for each scene polygon p, in front-to-back order relative to PLS
    bspTree->traverse(this->position, shadow_generator_loop, this);
    //endfor
    
    // discard SVBSPtree
    
    /*
    vector<Polygon> polygon_list;
    (this->SVBSPtree).get_polygons(polygon_list);
    bspTree->clear();
    bspTree->build(polygon_list);
    (this->SVBSPtree).clear();
    */
    
    // end proc
}