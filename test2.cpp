
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

using namespace std;

#include "Polygon.h"
#include "Utility.h"
#include "BSPTree.h"

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
			cout << " pov behine the plane";
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

	ifstream fin("testcases2");

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

	// for traversal test
	int pov_count = 0;
	fin >> pov_count;
	for (int i = 0; i < pov_count; i++) {
		float x, y, z;
		fin >> x >> y >> z;

		Point pov(x, y, z);
		tree.traverse(pov, traverse_test, NULL);

		cout << "POV: ";
		show_point(pov);
		switch (tree.get_position(pov)) {
			case BSPTree::OUTSIDE:
				cout << " Outside" << endl;
				break;
			case BSPTree::INSIDE:
				cout << " Inside" << endl;
				break;
			case BSPTree::ON_BOUNDARY:
				cout << " on boundary" << endl;
				break;
			case BSPTree::SPAN:
				cout << " span" << endl;
				break;
		}
	}

	fin.close();

	/*
	// reset for later testing
	tree.clear();
	tree.build(polygons);
	*/

	return 0;
}
