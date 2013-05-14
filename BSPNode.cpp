
#include <cstddef>
#include <vector>
#include <iostream>

using namespace std;

#include "Polygon.h"
#include "Utility.h"

#include "BSPNode.h"

///////////////////////////////////////////////////////////////////////////////

BSPNode::BSPNode(bool out)
{
	_is_outside = out;

	_front = NULL;
	_back = NULL;
	
	display_list = -1;
}

BSPNode::~BSPNode()
{
	// when the destructor is called, both subtrees should have been released,
	// so just set the two pointers to NULL
	clear();
}

void BSPNode::clear()
{
    delete _front; 
	delete _back; 
	_front = NULL; 
	_back = NULL; 
	_on_list.clear(); 
	_light_node.clear();
}

///////////////////////////////////////////////////////////////////////////////

void BSPNode::convert(const Polygon &fragment)
{
	// append the polygon to the 'on set'
	_on_list.push_back(fragment);
	
	_plane.n = fragment.normal();
	_plane.d = fragment[0].dot(_plane.n);

	// create two children, which are both leaves, to represent the two new
	// cells
	_front = new BSPNode(true);
	_back = new BSPNode(false);	// those behind are considered inside
}
	
void BSPNode::init_lighting()
{
	_light_node.clear();
	for (int i = 0; i < _on_list.size(); ++i)
		_light_node.fragments.push_back(_on_list[i]);
	
	//_fragments.clear();
	//for (int i = 0; i < _on_list.size(); ++i)
	//	_fragments.push_back(_on_list[i]);
	if (_front)
		_front->init_lighting();
	if (_back)
		_back->init_lighting();
}

void BSPNode::dump()
{
	cout << "BSPNode " << (void*)this << _plane.n << "," << _plane.d << endl;
	if (_front) _front->dump();
	else cout << "No front" << endl;
	for (int i = 0; i < _on_list.size(); ++i)
	{
		Polygon& fragment = _on_list[i];
		cout << "Fragment " <<fragment << endl;
	}
	_light_node.dump();
	/*for (int i = 0; i < _fragments.size(); ++i)
	{
		Polygon& fragment = _fragments[i];
		cout << "Fragment " <<fragment << endl;
	}*/
	if (_back) _back->dump();
	else cout << "No back" << endl;
	cout << "/BSPNode" << endl;
}

void BSPNode::add_polygon(const Polygon& polygon)
{
	// first check if the polygon should be added to the root of the subtree
	if (is_leaf()) {
		// the root node is empty, just add the polygon to it
		//printf("BSPNode(%p): converting leaf\n",this);
		convert(polygon);
	} else {
		// partitioning plane
			//printf("BSPNode(%p): splitting polygon by plane (%g,%g,%g,%g)\n",this,_plane.n.x,_plane.n.y,_plane.n.z,_plane.d);
			// split the polygon if it spans the partition plane
		Polygon front,back;

		polygon.split(front, back, _plane.n, _plane.d);
		
		if (!front.size() && !back.size()) // Coplanarish polygon
		{
			_back->add_polygon(polygon);
			//_on_list.push_back(polygon);
			// Find out if the polygon is really in front of or behind the plane
			//if ((_plane.n.dot(polygon.normal()) < 0 ? _plane.d + polygon.distance() : _plane.d - polygon.distance()) < 0)
				//_back->add_polygon(polygon);
			//else
			//	_on_list.push_back(polygon);
		}
		else
		{
			if (front.size())
				_front->add_polygon(front);
		
			if (back.size())
				_back->add_polygon(back);
		}
		//printf("BSPNode(%p): done\n",this);
	}
}
void BSPNode::add_polygons(const vector<Polygon>& polygons)
{
	for (int i = 0; i < polygons.size(); ++i)
		add_polygon(polygons[i]);
}


BSPNode* BSPNode::front() const
{
	return _front;
}

BSPNode* BSPNode::back() const
{
	return _back;
}

bool BSPNode::out() const
{
	return _is_outside;
}

bool BSPNode::is_leaf() const
{
	// a tree node is a leaf when both children are NULL
	return (_front == NULL && _back == NULL);
}

///////////////////////////////////////////////////////////////////////////////
	
bool BSPNode::traverse(bool frontfirst, const Point& eye, BSPNode::Callback callback, void* data)
{
	if (is_leaf())
		return callback(this,data);
	else
	{
		double d = _plane.n.dot(eye) - _plane.d;
		if (!frontfirst)
			d = -d;
		/*if (Utility::is_zero(d))
		{
			//printf("BSPNode(%p): Traversing front\n",this);
			if (!_front->traverse(frontfirst,eye,callback,data)) return false;
			//printf("BSPNode(%p): Traversing back\n",this);
			if (!_back->traverse(frontfirst,eye,callback,data)) return false;
		}
		else*/ if (d > 0)
		{
			//printf("BSPNode(%p): Traversing front\n",this);
			if (!_front->traverse(frontfirst,eye,callback,data)) return false;
			//printf("BSPNode(%p): Calling callback\n",this);
			if (!callback(this,data)) return false;
			//printf("BSPNode(%p): Traversing back\n",this);
			if (!_back->traverse(frontfirst,eye,callback,data)) return false;
		}
		else
		{
			//printf("BSPNode(%p): Traversing back\n",this);
			if (!_back->traverse(frontfirst,eye,callback,data)) return false;
			//printf("BSPNode(%p): Calling callback\n",this);
			if (!callback(this,data)) return false;
			//printf("BSPNode(%p): Traversing front\n",this);
			if (!_front->traverse(frontfirst,eye,callback,data)) return false;
		}
	}
	return true;
}
