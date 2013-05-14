
#include "Polygon.h"
#include "Utility.h"


using namespace std;

///////////////////////////////////////////////////////////////////////////////

Polygon::Polygon(const Point* points, int npoints) : _points(alloc_points(npoints)), _size(npoints), _n(0,0,0)
{
	// Newell's Method
	for (int i = 0; i < npoints; ++i)
	{
		_points[i] = points[i];
		const Point& p1 = points[i], &p2 = points[(i+1)%npoints];
		_n.x += (p1.y-p2.y) * (p1.z+p2.z);
		_n.y += (p1.z-p2.z) * (p1.x+p2.x);
		_n.z += (p1.x-p2.x) * (p1.y+p2.y);
	}
	_n = _n.normalize();
}

// Splits a fragment by a plane defined by normal and dist and fills 
void Polygon::split(Polygon& front, Polygon& back, const Vector3d& normal, double distance) const
{
	int i = 0;
	double lastdist, dist;
	Point* frontpts = alloc_points(_size+2);
	Point* backpts = alloc_points(_size+2);
	Point* frontptr = frontpts;
	Point* backptr = backpts;
	int nfront=0, nback=0;
	int lasti;
	front._size = 0;
	back._size = 0;
	lastdist = _points[lasti = _size-1].dot(normal) - distance;
	for (i = 0; i < _size; lasti = i++, lastdist = dist) // Walk around the polygon
	{
		dist = (*this)[i].dot(normal) - distance; // Check which side of the plane this point is on
		if (dist < -THRESHOLD) // Back
		{
			if (lastdist > THRESHOLD) // Last one was in front, add a split point
				*frontptr++ = *backptr++ = _points[i].interpolate(_points[lasti],dist/(dist-lastdist));
			++nback;
			*backptr++ = _points[i];
		}
		else if (dist > THRESHOLD) // Front
		{
			if (lastdist < -THRESHOLD) // Last one was in back, add a split point
				*frontptr++ = *backptr++ = _points[i].interpolate(_points[lasti],dist/(dist-lastdist));
			++nfront;
			*frontptr++ = _points[i];
		}
		else // Its on the plane, add it to both polygons
		{
			*frontptr++ = _points[i];
			*backptr++ = _points[i];
		}
	}
	if (nfront)
	{
		front._points = frontpts;
		front._size = frontptr-frontpts;
		front._n = _n;
	}
	else free(frontpts);
	if (nback)
	{
		back._points = backpts;
		back._size = backptr-backpts;
		back._n = _n;
	}
	else free(backpts);
}
std::ostream& operator<< (std::ostream& stream, const Point& p) 
{ 
	return stream << "(" << p.x << "," << p.y << "," << p.z << ")"; 
}
std::ostream& operator<< (std::ostream& stream, const Vector3d& v) 
{ 
	return stream << "<" << v.x << "," << v.y << "," << v.z << ">"; 
}
std::ostream& operator<< (std::ostream& stream, const Polygon& p) 
{ 
	stream << "[";
	for (int i = 0; i < p.size(); ++i)
		stream << p[i];
	return stream << "]"; 
}
	
void LightNode::dump()
{
	cout << "LightNode " << (void*)this <<endl;
	if (illuminated)
		illuminated->dump();
	if (shadowed)
		shadowed->dump();
	for (int i = 0; i < fragments.size(); ++i)
		cout << "Fragment " << fragments[i] << endl;
	cout << "/LightNode" << endl;
}
