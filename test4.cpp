//
//  test4.cpp
//  
//
//  Created by Aykut Simsek on 5/5/13.
//
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

using namespace std;

#include "Polygon.h"
#include "Utility.h"
#include "BSPTree.h"
#include "PointLightSource.h"

void show_point(const Point &p)
{
	cout << "(" << p.x << ", " << p.y << ", " << p.z << ")" << flush;
}

void show_polygon(const Polygon &p)
{
	show_point(p[0]);
	cout << "-";
	show_point(p[1]);
	cout << "-";
	show_point(p[2]);
}

void traverse_test(BSPTree::CallbackContext *ctx)
{
	if (ctx->is_leaf) {
		cout << "Leaf node,";
		if (ctx->is_out)
			cout << " outside" << endl;
		else
			cout << " inside" << endl;
	} else {
		cout << "Internal node,";
		if (ctx->is_before)
			cout << " pov before the plane";
		else
			cout << " pov behind the plane";
		show_polygon(ctx->plane);
		cout << endl;
        
		show_polygon(ctx->polygon);
		cout << endl;
	}
}

int main(int argc, char **argv)
{
	vector<Polygon> polygons;
	vector<Point> points;
    
	ifstream fin("testcases4");
    
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
        
		polygons.push_back(Polygon(points[id1], points[id2], points[id3]));
	}
    
	for (size_t i = 0; i < polygons.size(); i++) {
		show_polygon(polygons[i]);
		cout << endl;
	}
    
	// begin building the BSP tree
	BSPTree tree;
	for (size_t i = 0; i < polygons.size(); i++) {
		tree.add_polygon(polygons[i]);
	}
    
    
    Point viewPoint(0,0,20);
    //tree.traverse(viewPoint, traverse_test, NULL);
    //cout<<"!!!!!!!!!\n\n";
    
    BSPTree SVBSPTree;
    vector<Polygon> polygon_list;
    tree.get_polygons(polygon_list);
    SVBSPTree.build(polygon_list);
    
    // number of light sources
	int l = 0;
	fin >> l;
    
    vector<PointLightSource> lights;
    for (int i = 0; i < l; i++) {
		float x, y, z;
        float r, g, b;
		fin >> x >> y >> z;
        fin >> r >> g >> b;
        lights.push_back(PointLightSource(Point(x,y,z), Vector3d(0,0,0), Vector3d(r,g,b)));
	}
    
    fin.close();
    
    for(int i=0; i<lights.size(); i++)
    {
        cout<<"\n\nLight Source: "<<i<<"\n";
        (lights.at(i)).shadowGenerator(&SVBSPTree);
        (lights.at(i).SVBSPtree).traverse(viewPoint, traverse_test, NULL);
        //SVBSPTree.traverse(viewPoint, traverse_test, NULL);
    }
    //SVBSPTree.traverse(viewPoint, traverse_test, NULL);
    
	/*
     // reset for later testing
     tree.clear();
     tree.build(polygons);
     */
    
	return 0;
}
