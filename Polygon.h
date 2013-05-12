

#ifndef			CGEO_PROJECT_2_POLYGON_H
#define			CGEO_PROJECT_2_POLYGON_H

#include <vector>
#include <math.h>
#include <iostream>

#define THRESHOLD 0.00001

struct Point;

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
	
	float lengthsq() const { return x*x + y*y + z*z; }
	float length() const { return sqrt(lengthsq()); }
	Vector3d normalize() const { float l = length(); return Vector3d(x/l, y/l, z/l); }
	float dot(const Vector3d& b) const { return x*b.x + y*b.y + z*b.z; }
	Vector3d cross(const Vector3d& b) const { return Vector3d(y*b.z-z*b.y, z*b.x-x*b.z, x*b.y-y*b.x); }
};

/**
 * struct Point
 *
 * A Point represents a point in the scene and mostly is used to defined a
 * Polygon object. It has 3 coordinates because the scene is 3D.
 *
 * A Point object can be constructed by:
 * 1. providing the values of the coordinates
 * 2. copying the coordinates from another Point object
 * 3. [unknown]providing an instance of the point-type object used in the GUI
 *
 * Access to its coordinates is straightforward as all members are public
 **/
struct Point : Vector3d
{
	// - constructors
	// this one requires specific values for the coordiantes
	Point(float x, float y, float z) : Vector3d(x,y,z) {}

	// the copy constructor, which copys the valus from another Point object
	Point(const Point &copy) : Vector3d(copy) {}
	
	
	Vector3d operator-(const Point& b) const { return Vector3d(x-b.x,y-b.y,z-b.z); }
	
	Point interpolate(const Point& b, double t) const { return Point(x + (b.x-x)*t, y + (b.y-y)*t, z + (b.z-z)*t); }


};
std::ostream& operator<< (std::ostream& stream, const Point& p);
std::ostream& operator<< (std::ostream& stream, const Vector3d& v);

///////////////////////////////////////////////////////////////////////////////

struct Plane
{
	Vector3d n;
	float d;
	Plane(const Vector3d& n, float d) : n(n), d(d) {}
	Plane() : n(), d(0) { }
};

/**
 * class Polygon
 *
 * A Polygon represents 4 types of objects:
 * 1. A primitive of the scene
 * 2. A plane that partitions the subspace it resides into two smaller parts
 * 3. A fragment of another Polygon that is splitted by a partitioning plane
 * 4. A shadow plane of a shadow volume in a SVBSP tree
 *
 * Each Polygon object consists of three vertices specified during its con-
 * struction. To construct a Polygon object:
 * 1. specifying the three vertices(with Point objects)
 *
 * The vertices of a polygon can be accessed in a way similar to accessing
 * elements in an array. The vertices can not be altered once the polygon is
 * created.
 *
 * Since a Polygon also represents a plane, it has a normal vector that is
 * decided by its three vertices. So the order of these vertices will affect
 * the facing of the Polygon object. The right hand rule is used to decide the
 * normal vector, for now.
 *
 * For its usage as a fragment, the Polygon class also has a property indicates
 * whether it is lit by some light source.
 **/
class Polygon
{
public:
	// - constructors
	// this one requires three vertices
	Polygon(const Point &a, const Point &b, const Point &c) : _n((b-a).cross(c-b).normalize()) 
	{
		_points.push_back(a);
		_points.push_back(b);
		_points.push_back(c);
	}
	Polygon(const std::vector<Point>& points) : _points(points)
	{
		_n = (points[1]-points[0]).cross(points[2]-points[0]).normalize();
	}
	Polygon() {}
	
	// - copy constructor
	Polygon(const Polygon &copy): _points(copy._points), _n(copy._n), _lights(copy._lights) {}

	// - accessor to the vertices
	Point& 			operator[](int index) { return _points[index]; }
	const Point& 	operator[](int index) const { return _points[index]; }
	int size() const { return _points.size(); }
	
	void clear_lights() { _lights = 0; }
	bool has_light(int light) { return _lights & (1 << light); }
	void add_light(int light) { _lights |= (1 << light); }
	void remove_light(int light) { _lights &= ~(1 << light); }
	bool is_lit() { return _lights != 0; }
	
	const Vector3d& normal() const { return _n; }
	
	Plane plane() const { return Plane(_n,_points[0].dot(_n)); }
	
	void split(Polygon& front, Polygon& back, const Vector3d& normal, float dist) const;


private:
	// For splitting
	Polygon(const std::vector<Point>& points, const Polygon& original) : _points(points), _n(original._n), _lights(original._lights) {}
	
	// the three vertices
	//Point _vertices[3];
	std::vector<Point> _points;
	Vector3d _n;
	unsigned int _lights; // Bitmask of lights

	// the status of being lit
};
std::ostream& operator<< (std::ostream& stream, const Polygon& p);

#endif
