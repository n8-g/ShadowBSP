
#include "Polygon.h"
#include "Utility.h"


using namespace std;

///////////////////////////////////////////////////////////////////////////////

// Splits a polygon by a plane defined by normal and dist and fills 
void Polygon::split(Polygon& front, Polygon& back, const Vector3d& normal, double distance) const
{
	int i = 0;
	int size = _points.size();
	front = Polygon(vector<Point>(),*this);
	back = Polygon(vector<Point>(),*this);
	double lastdist, dist, lasti;
	int nfront=0, nback=0;
	lastdist = _points[lasti = size-1].dot(normal) - distance;
	for (i = 0; i < size; lasti = i++, lastdist = dist) // Walk around the polygon
	{
		dist = _points[i].dot(normal) - distance; // Check which side of the plane this point is on
		if (dist < -THRESHOLD) // Back
		{
			if (lastdist > THRESHOLD) // Last one was in front, add a split point
			{
				Point split = _points[i].interpolate(_points[lasti],dist/(dist-lastdist));
				front._points.push_back(split);
				back._points.push_back(split);
			}
			++nback;
			back._points.push_back(_points[i]);
		}
		else if (dist > THRESHOLD) // Front
		{
			if (lastdist < -THRESHOLD) // Last one was in back, add a split point
			{
				Point split = _points[i].interpolate(_points[lasti],dist/(dist-lastdist));
				front._points.push_back(split);
				back._points.push_back(split);
			}
			++nfront;
			front._points.push_back(_points[i]);
		}
		else // Its on the plane, add it to both polygons
		{
			front._points.push_back(_points[i]);
			back._points.push_back(_points[i]);
		}
	}
	if (!nfront) front._points.clear(); // All points in front are planar, just clear it
	if (!nback) back._points.clear(); // All points in back are planar, just clear it
	/*
	Point points[3] = {_a,_b,_c};
	double distances[3] = {_a.dot(normal),_b.dot(normal),_c.dot(normal)};
	if (distances[0] >= dist-THRESHOLD && distances[1] >= dist-THRESHOLD && distances[2] >= dist-THRESHOLD) // No need to split
		front.push_back(*this);
	else if (distances[0] <= dist+THRESHOLD && distances[1] <= dist+THRESHOLD && distances[2] <= dist+THRESHOLD) // No need to split
		back.push_back(*this);
	else
	{
		for (int i = 0; i < 3; ++i)
		{
			int j = (i + 1) % 3;
			int k = (j + 1) % 3;
			if (distances[i] < dist+THRESHOLD || distances[j] >= dist+THRESHOLD) continue;
			// point i is in front of the plane and point j is behind the plane
			if (distances[k] >= dist-THRESHOLD) // k is on or in front of the plane
			{
				Point ij = points[i].interpolate(points[j],(dist-distances[j])/(distances[i]-distances[j]));
				Point kj = points[k].interpolate(points[j],(dist-distances[j])/(distances[k]-distances[j]));
				Polygon f1(points[i],kj,points[k],*this);
				Polygon b1(ij,points[j],kj,*this);
				cout << "New front fragment: " << f1 << endl;
				cout << "New back fragment: " << b1 << endl;
				front.push_back(f1);
				back.push_back(b1);
				if (distances[k] > dist+THRESHOLD)
				{
					Polygon f2(points[i],kj,points[k],*this);
					cout << "New front fragment: " << f2 << endl;
					front.push_back(f2);
				}
			}
			else
			{
				Point ij = points[i].interpolate(points[j],(dist-distances[j])/(distances[i]-distances[j]));
				Point ik = points[i].interpolate(points[k],(dist-distances[k])/(distances[i]-distances[k]));
				printf("New front fragment: "); dump(points[i]); dump(ij); dump(ik); printf("\n");
				printf("New back fragment: "); dump(ij); dump(points[j]); dump(points[k]); printf("\n");
				printf("New back fragment: "); dump(points[j]); dump(points[k]); dump(ik); printf("\n");
				front.push_back(Polygon(points[i],ij,ik,*this));
				back.push_back(Polygon(ij,points[j],points[k],*this));
				back.push_back(Polygon(points[j],points[k],ik,*this));
			}
			return;
		}
	}
	*/
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

