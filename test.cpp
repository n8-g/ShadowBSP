
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

using namespace std;

#include "Polygon.h"
#include "Utility.h"

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

int main(int argc, char **argv)
{
	// m polygons, n points
	// p1 of polygon 1
	// p2 of polygon 1
	// .....
	// p1 of polygon 2
	// ....
	// p1 of polygon m
	// p2 of polygon m
	// p3 of polygon m
	// test point 1
	// test point 2
	// ...
	// test point n
	
	// number of test2
	// index of plane
	// index of polygon 1
	// index of polygon 2
	// ...
	// index of last polygon
	
	// number of test3
	// index of plane
	// p1 for line 1
	// p2 for line 1
	// ...
	// p1 for last line
	// p2 for last line
	
	// number of test4
	// index of polygon being splitted
	// index of plane 1
	
	int m = 0;
	int n = 0;
	int test_2 = 0;
	int test_3 = 0;
	int test_4 = 0;

	vector<Polygon> polygons;
	vector<Point> points;

	ifstream fin("testcases");

	// read in the polygons and points for tests
	fin >> m >> n;
	for (int i = 0; i < m; i++) {
		float x1, y1, z1;
		float x2, y2, z2;
		float x3, y3, z3;

		fin >> x1 >> y1 >> z1;
		fin >> x2 >> y2 >> z2;
		fin >> x3 >> y3 >> z3;
		polygons.push_back(Polygon(Point(x1, y1, z1), Point(x2, y2, z2),
				Point(x3, y3, z3)));

	}

	for (int i = 0; i < n; i++) {
		float x, y, z;

		fin >> x >> y >> z;
		points.push_back(Point(x, y, z));
	}

	// show polygons and points
	for (size_t i = 0; i < polygons.size(); i++) {
		show_polygon(polygons[i]);
		cout << endl;
	}
	for (size_t i = 0; i < points.size(); i++) {
		show_point(points[i]);
		cout << endl;
	}

	// test 1
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			cout << setw(5) 
				<< Utility::coplanar_det(polygons[i], points[j]) << " ";
		}
		cout << endl;
	}

	// test 2
	int plane_idx;
	fin >> test_2;
	fin >> plane_idx;
	Polygon plane = polygons[plane_idx];
	for (int i = 0; i < test_2; i++) {
		int idx = 0;
		fin >> idx;
		Polygon polygon = polygons[idx];
		if (Utility::coplanar(polygon, plane)) {
			cout << "coplanar";
		} else if (Utility::before(polygon, plane)) {
			cout << "before";
		} else if (Utility::behind(polygon, plane)) {
			cout << "behind";
		} else {
			cout << "span";
		}
		cout << endl;
	}

	// test 3
	fin >> test_3;
	fin >> plane_idx;
	plane = polygons[plane_idx];
	for (int i = 0; i < test_3; i++) {
		float x1, y1, z1;
		float x2, y2, z2;
		fin >> x1 >> y1 >> z1;
		fin >> x2 >> y2 >> z2;
		Point a(x1, y1, z1);
		Point b(x2, y2, z2);
		Point intersect = Utility::intersection(plane, a, b);

		show_point(intersect);
	}

	cout << endl;

	// test 4
	fin >> test_4;
	fin >> plane_idx;
	plane = polygons[plane_idx];
	for (int i = 0; i < test_4; i++) {
		vector<Polygon> front_list;
		vector<Polygon> back_list;

		int idx = 0;
		fin >> idx;
		Utility::split_polygon(plane, polygons[idx], front_list, back_list);

		cout << "FRONT: " << endl;
		for (size_t j = 0; j < front_list.size(); j++) {
			show_polygon(front_list[j]);
			cout << endl;
		}

		cout << "BACK: " << endl;
		for (size_t j = 0; j < back_list.size(); j++) {
			show_polygon(back_list[j]);
			cout << endl;
		}
	}

	fin.close();

	return 0;
}
