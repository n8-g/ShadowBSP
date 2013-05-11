

#ifndef			CGEO_PROJECT_2_POLYGON_H
#define			CGEO_PROJECT_2_POLYGON_H

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
struct Point
{
	// the (x, y, z) coordinates
	float x;
	float y;
	float z;

	// - constructors
	// this one requires specific values for the coordiantes
	Point(float x, float y, float z);

	// the copy constructor, which copys the valus from another Point object
	Point(const Point &copy);

	// the 3rd one is unknown, for now
};

///////////////////////////////////////////////////////////////////////////////

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
	Polygon(const Point &a, const Point &b, const Point &c);
	
	// - copy constructor
	Polygon(const Polygon &copy);

	// - accessor to the vertices
	Point& 			operator[](int index);
	const Point& 	operator[](int index) const;

	// - get-property methods
	inline bool is_lit() const;

	// - set-property methods
	inline void set_lit(bool is_lit);
    
    /*
    bool is_valid()
    {
        if (this->_a && this->_b && this->_c)
            return true;
        return false;
    }*/

private:
	// the three vertices
	//Point _vertices[3];
	Point _a;
	Point _b;
	Point _c;

	// the status of being lit
	bool _is_lit;
};

// - inline functions
bool Polygon::is_lit() const
{
	return _is_lit;
}

void Polygon::set_lit(bool is_lit)
{
	_is_lit = is_lit;
}

#endif
