
#ifndef			CGEO_PROJECT_2_BSPTREE_H
#define			CGEO_PROJECT_2_BSPTREE_H

#include <vector>

#include "Polygon.h"
#include "BSPNode.h"


/**
 * class BSPTree
 *
 * A BSPTree is a binary tree representing a partitioning of the scene and the
 * objects in it. It also represents a BSP tree for shadow volumes. The scene 
 * is stored in the tree as the collection of its polygons.
 *
 * A BSPTree is empty at the beginning, and Polygons are added to it incremen-
 * tally. A list of these polygons can be acquired later by calling a member
 * function.
 *
 * A front-to_back traversal framework is provided to make use of the BSP tree.
 **/
class BSPTree
{
public:
	/**
	 * struct CallbackContext
	 *
	 * A CallbackContext contains data that may be required by callback 
	 * functions * for the preorder traversal of a BSPTree
	 **/
	struct CallbackContext
	{
		// the point of view
		Point* pov;

		// the plane associated with the current node
		Plane plane;

		// the polygon being accessed
		Polygon* polygon;

		// whether the current node is internal node or leaf node
		bool is_leaf;

		// whether the pov is in front of, or behind the plane
		// NOTE: if pov is on the plane, those polygons will not be processed
		// by the callback, therefore those in this structure must be either 
		// before or behind the plane
		//
		// only useful when the current node is not a leaf
		bool is_before;

		// whether the cell is outside/inside a volume
		// NOTE: only useful when the current node is a leaf
		bool is_out;

		// pointer to user custom data
		void *user_data;

        //BSPNode * get_root(){ return _root;}
        
		// - constructor
		// this is used to make sure a 'default' version of Polygon constructor
		// isn't needed
		CallbackContext(const Point &the_pov, 
				const Plane &the_plane, const Polygon &the_polygon, 
				bool is_leaf, bool is_before, bool is_out);
	};

	// type of the callback function:
	// 		void Callback(CallbackContext *ctx);
	typedef void (*InorderCallback)(BSPTree::CallbackContext* context);

	enum PositionType {
		OUTSIDE,
		INSIDE,
		ON_BOUNDARY,
		SPAN
	};

public:
	// - the constructor
	BSPTree();

	// - the destructor
	~BSPTree();

	// - to build a complete BSP tree from a collection of Polygons
	// NOTE: the input list will be altered (by changing the order of its 
	// elements randomly)
	void build(std::vector<Polygon> &list);

	// - to add a Polygon to the BSP tree
	void add_polygon(const Polygon &polygon);

	// - to remove all nodes in the BSP tree and reset it to empty
	// NOTE: when the tree is empty, it means it has only one empty root node
	void clear();

	// - the inorder traversal framework
	// nodes are visited in a front-to-back order respect to a obsevation point 
	void traverse(const Point &pov, InorderCallback callback, 
			void *user_data = NULL);

	// - another traversal framework
	void filter_polygon(const Polygon &polygon, InorderCallback callback,
			void *user_data = NULL);

	// - to acquire the list of Polygons
	void get_polygons(std::vector<Polygon> &list);

	// - to test if a polygon is completed inside/outside the volume of the
	// tree, or on any of the boundary planes, or span it
	PositionType get_position(const Polygon &polygon) const;
	PositionType get_position(const Point &point) const;
	
	bool traverse(const Point& eye, BSPNode::Callback callback, void* data);

private:
	// the root node of the tree
	BSPNode *_root;

private:
	// private helper functions
	
	// - filter down and add the polygon to the tree, recursively
	void _add_polygon_to_subtree(BSPNode *root, const Polygon &polygon);

	// - release the subtrees recursively
	void _clear_subtree(BSPNode *root);

	// - visit subtrees in front-to-back order, and process tree nodes with a
	// callback function
	void _visit_subtree_inorder(BSPNode *root, const Point &pov,
			BSPTree::InorderCallback callback, void *user_data);

	//
	void _filter_subtree(BSPNode *root, const Polygon &polygon,
			BSPTree::InorderCallback callback, void *user_data);

	// - invoke the callback function to process the polygons on the plane of
	// the current tree node
	void _invoke_callback(BSPNode *node, const Point &pov, 
			BSPTree::InorderCallback callback, void *user_data);

	void _invoke_callback(BSPNode *node, const Polygon &polygon,
			BSPTree::InorderCallback callback, void *user_data);

	// - collect polygons from subtree
	// use preorder traversal
	void _collect_polygons(BSPNode *root, std::vector<Polygon> &list);

	//
	PositionType _check_position(BSPNode *root, const Polygon &polygon) const;
	PositionType _check_position(BSPNode *root, const Point &point) const;
};

#endif
