
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
	// the Fragment will also represent the partition plane
	void convert(const Polygon &fragment);

	// get-xxx functions
	BSPNode* 				front() const;		// those in front of the plane
	BSPNode* 				back() const;		// those behind the plane
    
	bool 					out() const;		// whether its cell is outside
	bool					is_leaf() const;	// whether is a leave node

	const Plane& 				plane() const { return _plane; }		// the partitioning plane

	std::vector<Polygon>&		polygons()	{ return _on_list; }	// the polygons on the plane
	const std::vector<Polygon>& polygons() const { return _on_list; }

	//std::vector<Polygon>&		fragments() { return _fragments; }		
	//const std::vector<Polygon>& fragments() const { return _fragments; }
	LightNode& light_node() { return _light_node; }
	
	// OpenGL display list for optimized rendering
	unsigned int display_list;
	
	void clear();
	void add_polygon(const Polygon& polygon);
	
	void add_polygons(const std::vector<Polygon>& polygons);
	
	// Initializes _light_node for this node and all children
	void init_lighting();
	
	typedef bool (*Callback) (BSPNode* node, void* data);
	bool traverse(bool frontfirst, const Point& eye, BSPNode::Callback callback, void* param);
	
	void dump();
private:
	// the partitioning plane
	//Polygon _plane;

	// the list of gragments that are on the plane.
	std::vector<Polygon> _on_list;
	LightNode _light_node;
	
	Plane _plane;

	// the two children
	BSPNode *_front;
	BSPNode *_back;

	// status flags
	bool _is_outside;	// whether the cell of the node is outside
};

#endif
