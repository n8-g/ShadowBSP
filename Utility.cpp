#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>

using namespace std;

#include "Polygon.h"
#include "Utility.h"

///////////////////////////////////////////////////////////////////////////////

float Utility::_threshold = THRESHOLD;

float Utility::coplanar_det(const Point &a, const Point &b, const Point &c, 
		const Point &p)
{
	// the determinant for coplanar test is:
	// 		| xa, ya, za, 1 |
	// 		| xb, yb, zb, 1 |
	// 		| xc, yc, zc, 1 |
	// 		| xp, yp, zp, 1 |
	
	// let d1 be
	// | xb yb zb |
	// | xc yc zc |
	// | xp yp zp |
	float d1 = b.x * c.y * p.z + b.y * c.z * p.x + b.z * c.x * p.y
		- b.x * c.z * p.y - b.y * c.x * p.z - b.z * c.y * p.x;

	// let d2 be
	// | xa ya za |
	// | xc yc zc |
	// | xp yp zp |
	float d2 = a.x * c.y * p.z + a.y * c.z * p.x + a.z * c.x * p.y
		- a.x * c.z * p.y - a.y * c.x * p.z - a.z * c.y * p.x;
	
	// let d3 be
	// | xa ya za |
	// | xb yb zb |
	// | xp yp zp |
	float d3 = a.x * b.y * p.z + a.y * b.z * p.x + a.z * b.x * p.y
		- a.x * b.z * p.y - a.y * b.x * p.z - a.z * b.y * p.x;
	
	// let d4 be
	// | xa ya za |
	// | xb yb zb |
	// | xc yc zc |
	float d4 = a.x * b.y * c.z + a.y * b.z * c.x + a.z * b.x * c.y
		- a.x * b.z * c.y - a.y * b.x * c.z - a.z * b.y * c.x;
	
	// then the determinant will be 
	// d4 - d3 + d2 - d1
	

	return d4 - d3 + d2 - d1;
}

float Utility::coplanar_det(const Polygon &f, const Point &p)
{
	// just call the point version
	return coplanar_det(f[0], f[1], f[2], p);
}

bool Utility::before(const Polygon &a, const Polygon &b)
{
	// make sure a and b is not coplanar
	if (coplanar(a, b))
		return false;

	// a point is before a plane if the coplanar determinant is negative
	float det_a = coplanar_det(b, a[0]);
	float det_b = coplanar_det(b, a[1]);
	float det_c = coplanar_det(b, a[2]);
	
	// polygon a is before polygon b iff the three vertices of a are all
	// before b.
	// it is okey for one or two of them to be on the plane
	/*
	return (is_nonpositive(det_a) && is_nonpositive(det_b) 
			&& is_nonpositive(det_c));
	*/
	return (is_nonnegative(det_a) && is_nonnegative(det_b)
			&& is_nonnegative(det_c));
}

bool Utility::before(const Point &a, const Polygon &plane)
{
	float det = coplanar_det(plane, a);

	// before if positive
	return (!is_zero(det) && is_nonnegative(det));
}

bool Utility::behind(const Polygon &a, const Polygon &b)
{
	// make sure a and b is not coplanar
	if (coplanar(a, b))
		return false;

	// a point is behind a plane if the coplanar determinant is positive
	float det_a = coplanar_det(b, a[0]);
	float det_b = coplanar_det(b, a[1]);
	float det_c = coplanar_det(b, a[2]);
	
	// polygon a is behind polygon b iff the three vertices of a are all
	// behind b.
	// it is okey for one or two of them to be on the plane
	/*
	return (is_nonnegative(det_a) && is_nonnegative(det_b)
			&& is_nonnegative(det_c));
	*/
	return (is_nonpositive(det_a) && is_nonpositive(det_b)
			&& is_nonpositive(det_c));
}

bool Utility::behind(const Point &a, const Polygon &plane)
{
	float det = coplanar_det(plane, a);

	// behind if negative
	return (!is_zero(det) && is_nonpositive(det));
}

bool Utility::coplanar(const Polygon &a, const Polygon &b)
{
	// a point is on a plane if the coplanar determinant is zero
	float det_a = coplanar_det(b, a[0]);
	float det_b = coplanar_det(b, a[1]);
	float det_c = coplanar_det(b, a[2]);

	return (is_zero(det_a) && is_zero(det_b) && is_zero(det_c));
}

Point Utility::intersection(const Polygon &f, const Point &a, const Point &b)
{
	// using the formula from wikipedia:
	// point on the line ab:
	// 	a + (b-a)*t
	// point on the plane defined by the polygon f:
	//  p[0] + (p[1] - p[0])*u + (p[2] - p[0])*v
	// so the intersection can be computed with
	// 	| Xa - Xb	X1 - X0		X2 - X0 | |t|	|Xa - X0|
	// 	| Ya - Yb	Y1 - Y0		Y2 - Y0 | |u| = |Ya - Y0|
	// 	| Za - Zb	Z1 - Z0		Z2 - Z0 | |v|	|Za - Z0|
	
	// first column of the matrix; 'dt_' means difference for parameter t
	float dtx = a.x - b.x;
	float dty = a.y - b.y;
	float dtz = a.z - b.z;

	// second column of the matrix; 'du_' means difference for parameter u
	float dux = f[1].x - f[0].x;
	float duy = f[1].y - f[0].y;
	float duz = f[1].z - f[0].z;

	// third column of the matrix; 'dv_' means difference for parameter v
	float dvx = f[2].x - f[0].x;
	float dvy = f[2].y - f[0].y;
	float dvz = f[2].z - f[0].z;

	// the b vector
	float b1 = a.x - f[0].x;
	float b2 = a.y - f[0].y;
	float b3 = a.z - f[0].z;

	// now the equation becomes
	// | dtx dux dvx | |t|	 |b1|
	// | dty duy dvy | |u| = |b2|
	// | dtz duz dvz | |v|	 |b3|

	// denominator for Cramer's rule
	float d = dtx * duy * dvz + dux * dvy * dtz + dvx * dty * duz
		- dtx * dvy * duz - dux * dty * dvz - dvx * duy * dtz;

	// numerator for Cramer's rule
	float n = b1 * duy * dvz + dux * dvy * b3 + dvx * b2 * duz
		- b1 * dvy * duz - dux * b2 * dvz - dvx * duy * b3;
	
	// just computing t will be enough
	double t = n / d;

	// use the parameter to compute the coordinates
	// '-', instead of '+' is used here because dtx=ax-bx, while bx-ax is
	// needed here
	Point p(a.x - dtx * t,
			a.y - dty * t,
			a.z - dtz * t);

	return p;
}

bool Utility::is_zero(float v)
{
	return (v < _threshold && v > -_threshold);
}

bool Utility::is_nonpositive(float v)
{
	return (v <= _threshold);
}

bool Utility::is_nonnegative(float v)
{
	return (v >= -_threshold);
}

void Utility::permute_list(vector<Polygon> &list)
{
	// pick a new seed
	srand((unsigned int) time(NULL));

	size_t len = list.size();
	for (size_t i = 0; i < len; i++) {
		// pick two random elements (it may pick the same one twice)
		size_t idx_a = (size_t)(rand() % len);
		size_t idx_b = (size_t)(rand() % len);

		// swap the two elements
		Polygon temp = list[idx_a];
		list[idx_a] = list[idx_b];
		list[idx_b] = temp;
	}
}

void Utility::split_polygon(const Polygon &polygon, const Polygon &plane,
		vector<Polygon> &front_list, vector<Polygon> &back_list)
{
	// find the first cross edge
	// a cross edge is an edge whose two end points lie on different sides of 
	// the plane. If a polygon spans a plane, then there exists at least one
	// such edge
	pair<int, int> cross_edge = Utility::_cross_edge(polygon, plane);

	// now we have two points from the polygon
	Point a(polygon[cross_edge.first]);
	Point b(polygon[cross_edge.second]);

	// and these two points must intersect the plane
	Point i1 = Utility::intersection(plane, a, b);

	// find out the third point
	// exploit that the points are ordered
	int idx_c = (cross_edge.second + 1) % 3;
	Point c(polygon[idx_c]);

	// the position of the third point decide the pattern of splitting
	float det_a = coplanar_det(plane, a);
	float det_c = coplanar_det(plane, c);
	if (Utility::is_zero(det_c)) {
		// point c is on the plane
		// so there is no more intersection points
		//if (Utility::is_nonpositive(det_a)) {
		if (Utility::is_nonnegative(det_a)) {
			// point a is in front of the plane
			// front list include: (a, i1, c)
			front_list.push_back(Polygon(a, i1, c));

			// back list include: (b, c, i1)
			back_list.push_back(Polygon(b, c, i1));
		} else {
			// point a is behind the plane
			// front list include: (b, c, i1)
			front_list.push_back(Polygon(b, c, i1));

			// back list include: (a, i1, c)
			back_list.push_back(Polygon(a, i1, c));
		}
	} else if (Utility::is_nonpositive(det_a * det_c)) {
		// point c is opposite of point a
		// so there is a second intersection point on edge ac
		Point i2 = Utility::intersection(plane, a, c);

		//if (Utility::is_nonpositive(det_a)) {
		if (Utility::is_nonnegative(det_a)) {
			// point a is in front of the plane
			// front list include: (a, i1, i2)
			front_list.push_back(Polygon(a, i1, i2));

			// back list include: (b, i2, i1), (b, c, i2)
			back_list.push_back(Polygon(b, i2, i1));
			back_list.push_back(Polygon(b, c, i2));
		} else {
			// point a is behind the plane
			// front list include: (b, i2, i1), (b, c, i2)
			front_list.push_back(Polygon(b, i2, i1));
			front_list.push_back(Polygon(b, c, i2));

			// back list include: (a, i1, i2)
			back_list.push_back(Polygon(a, i1, i2));
		}
	} else {
		// point c is on the same side of point a
		// so there is a second intersection point on edge bc
		Point i2 = Utility::intersection(plane, b, c);

		//if (Utility::is_nonpositive(det_a)) {
		if (Utility::is_nonnegative(det_a)) {
			// point a is in front of the plane
			// front list include: (a, i1, c), (c, i1, i2)
			front_list.push_back(Polygon(a, i1, c));
			front_list.push_back(Polygon(c, i1, i2));

			// back list include: (b, i2, i1)
			back_list.push_back(Polygon(b, i2, i1));
		} else {
			// point a is behind the plane
			// front list include: (b, i2, i1)
			front_list.push_back(Polygon(b, i2, i1));

			// back list include: (a, i1, c), (c, i1, i2)
			back_list.push_back(Polygon(a, i1, c));
			back_list.push_back(Polygon(c, i1, i2));
		}
	}
}

int  Utility::orientation(const Polygon &a, const Point &p){
    float orin_angle = orientation_angle(a,p);
    if (orin_angle < 0.0)
    {
        // (* Orientaion is below plane *)
        return -1; 
    }
    else if(orin_angle > 0.0)
    {
        // (* Orientaion is above plane *)
        return 1; 
    }
    else
    {
        // (* Orientaion is coplanar to plane if result is 0 *)
        return 0; 
    }
}

float Utility::orientation_angle(const Polygon &a, const Point &p){
    
    double orientation;
    double px1, px2, px3, py1, py2, py3, pz1, pz2, pz3;

    px1 = a[0].x - p.x;
    px2 = a[1].x - p.x;
    px3 = a[2].x - p.x;
    
    py1 = a[0].y - p.y;
    py2 = a[1].y - p.y;
    py3 = a[2].y - p.y;
    
    pz1 = a[0].z - p.z;
    pz2 = a[1].z - p.z;
    pz3 = a[2].z - p.z;
    
    orientation = px1 * ( py2 * pz3 - pz2 * py3) + px2 * (py3 * pz1 - pz3 * py1) + px3 * (py1 * pz2 - pz1 * py2);
    return orientation;
}
///////////////////////////////////////////////////////////////////////////////

pair<int, int> Utility::_cross_edge(const Polygon &polygon, 
		const Polygon &plane)
{
	// assuming at least one cross edge exists
	for (int i = 0; i < 3; i++) {
		// indices for the two comparing points
		int idx_1 = i;
		int idx_2 = (i + 1) % 3;

		// compute their coplanar test determinants
		float det_1 = coplanar_det(plane, polygon[idx_1]);
		float det_2 = coplanar_det(plane, polygon[idx_2]);

		// check if they lies on different sides of the plane (which makes the
		// determinants has different signs)
		// NOTE: the first two tests make sure neither point is on the plane(
		// floating point may cause their product to be negative while one is
		// on the plane)
		if (!Utility::is_zero(det_1) && !Utility::is_zero(det_2) &&
				Utility::is_nonpositive(det_1 * det_2)) {
			// found the edge
			return pair<int, int>(idx_1, idx_2);
		}
	}

	// in case there isn't a cross edge (which should never happen), just 
	// return the first vertex (twice)
	return pair<int, int>(0, 0);
}

float Utility::_det3b3(float matrix[])
{
	// a 3x3 matrix:
	// | a11 a12 a13 |
	// | a21 a22 a23 |
	// | a31 a32 a33 |
//	float a11 = matrix
	
	return 0.0;
}


