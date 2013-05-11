

#ifndef		__CGEO_PROJECT_2_UTILITY__
#define		__CGEO_PROJECT_2_UTILITY__

#include <vector>
#include <utility>

class Point;
class Polygon;

// some helper functions
class Utility
{
public:
	// - compute the value of the determinant used to decide if 4 points are
	// coplanar
	// point a, b, c define a plane, and point p is tested to see if it is on
	// that plane
	static float coplanar_det(const Point &a, const Point &b, const Point &c,
		   	const Point &p);

	// this one use a polygon to replace the three points
	static float coplanar_det(const Polygon &f, const Point &p);

	// - test whether polygon a is in front of polygon b
	static bool before(const Polygon &a, const Polygon &b);

	static bool before(const Point &a, const Polygon &plane);

	// - test whether polygon a is behind polygon b
	static bool behind(const Polygon &a, const Polygon &b);

	static bool behind(const Point &a, const Polygon &plane);

	// - test whether polygon a and b are coplanar
	static bool coplanar(const Polygon &a, const Polygon &b);

	// - compute the intersection of a polygon and a segment between two points
	// assuming that the segment always intersects with the polygon
	static Point intersection(const Polygon &f, const Point &a, const Point &b);

	// - to test if provided floating point value should be considered zero
	static bool is_zero(float v);

	// - to test if provided floating point value is non-positive
	static bool is_nonpositive(float v);

	// - to test if provided floating point vlaue is non-negative
	static bool is_nonnegative(float v);

	// - permute the list to randomize the order of its elements
	static void permute_list(std::vector<Polygon> &list);

	// - split a polygon with a plane, then store the fragments into two lists
	// according to their relative position to the plane
	static void split_polygon(const Polygon &polygon, const Polygon &plane,
			std::vector<Polygon> &front_list, std::vector<Polygon> &back_list);
    
    static int orientation(const Polygon &a, const Point &p);
    
    static float orientation_angle(const Polygon &a, const Point &p);
private:
	// for floating point comparison
	float static _threshold;

private:
	// - find the vertices of the first corss edge
	// the ordering between the points returned is the same as that in the
	// polygon
	// returning the indices to the points in the polygon
	static std::pair<int, int> _cross_edge(const Polygon &polygon,
			const Polygon &plane);

	// - compute the value of a 3x3 determinant
	static float _det3b3(float matrix[]);
};

#endif
