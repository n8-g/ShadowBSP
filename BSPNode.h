
#ifndef			CGEO_PROJECT_2_BSPNODE_H
#define			CGEO_PROJECT_2_BSPNODE_H

#include <vector>

#include "Polygon.h"

/**
 * class BSPNode
 *
 * A BSPNode is a tree node of the BSP tree. If it is an internal node, it re-
 * presents the partitioning plane of the corresponding subspace, and contains
 * the Polygons inside that plane, too. If it is a leaf node, it represents a
 * cell in the subdivided space.
 *
 * Each internal BSP node has two children, one points to those in front of the
 * partitioning plane, one points to those behind it.
 *
 * All BSPNode are created as leaves first, and later be converted into inter-
 * nal nodes(because at the beginning there isn't any partitioning).
 **/
class BSPNode
{
public:
	// - constructor
	// 	'out' means the cell of this node is in front of some planes, or there
	// 	is no partitions
	BSPNode(bool out = true);

	// - destructor
	~BSPNode();

	// - convert this node into an internal node
	// the Polygon will also represent the partition plane
	void convert(const Polygon &polygon);

	// - add a polygon(that is on the plane)
	void add_polygon(const Polygon &polygon);

	// get-xxx functions
	BSPNode* 				front() const;		// those in front of the plane
	BSPNode* 				back() const;		// those behind the plane

    inline void set_front(BSPNode* front);
    inline void set_back(BSPNode* back);
    
	bool 					out() const;		// whether its cell is outside
	bool					is_leaf() const;	// whether is a leave node

	Polygon 				plane() const;		// the partitioning plane

	std::vector<Polygon>&		polygons();			// the polygons on the plane
	const std::vector<Polygon>& polygons() const;
   
private:
	// the partitioning plane
	//Polygon _plane;

	// the list of polygons that are on the plane.
	// the partitioning plane will be the first polygon in the list
	std::vector<Polygon> _on_list;

	// the two children
	BSPNode *_front;
	BSPNode *_back;

	// status flags
	bool _is_outside;	// whether the cell of the node is outside
};

// - inline functions
void BSPNode::set_front(BSPNode* front)
{
	this->_front = front;
}

void BSPNode::set_back(BSPNode* back)
{
	this->_back = back;
}

#endif
