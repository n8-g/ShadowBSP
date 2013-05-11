
#include <cstddef>
#include <vector>

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
}

BSPNode::~BSPNode()
{
	// when the destructor is called, both subtrees should have been released,
	// so just set the two pointers to NULL
	_front = NULL;
	_back = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BSPNode::convert(const Polygon &polygon)
{
	// append the polygon to the 'on set'
	_on_list.push_back(polygon);

	// create two children, which are both leaves, to represent the two new
	// cells
	_front = new BSPNode(true);
	_back = new BSPNode(false);	// those behind are considered inside
}

void BSPNode::add_polygon(const Polygon &polygon)
{
	// just append it to the 'on set'
	_on_list.push_back(polygon);
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

Polygon BSPNode::plane() const
{
	// the first element of the 'on set' is the partitioning plane
	if (_on_list.empty()) {
		return Polygon(Point(0.0, 0.0, 0.0), Point(0.0, 0.0, 0.0), Point(0.0, 0.0, 0.0));
	}

	return _on_list[0];
}

vector<Polygon>& BSPNode::polygons() 
{
	return _on_list;
}

const vector<Polygon>& BSPNode::polygons() const
{
	return _on_list;
}
