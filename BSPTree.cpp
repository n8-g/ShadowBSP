
#include <cstddef>
#include <vector>

using namespace std;

#include "Polygon.h"
#include "Utility.h"
#include "BSPNode.h"

#include "BSPTree.h"

///////////////////////////////////////////////////////////////////////////////

BSPTree::CallbackContext::CallbackContext(const Point &the_pov, 
		const Polygon &the_plane, const Polygon &the_polygon, 
		bool is_leaf, bool is_before, bool is_out)
: pov(the_pov), plane(the_plane), polygon(the_polygon)
{
	this->is_leaf = is_leaf;
	this->is_before = is_before;
	this->is_out = is_out;

	this->user_data = NULL;
}

///////////////////////////////////////////////////////////////////////////////

BSPTree::BSPTree()
{
	// just create an empty BSPNode marked as 'out'
	_root = new BSPNode(true);
}

BSPTree::~BSPTree()
{
	// clear all tree nodes
	clear();

	_root = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BSPTree::build(vector<Polygon> &list)
{
	// reorder the list randomly
	Utility::permute_list(list);

	// iterate through the randomized list and add each element to the tree
	vector<Polygon>::iterator iter = list.begin();
	while (iter != list.end()) {
		_add_polygon_to_subtree(_root, *iter);

		++iter;
	}
}

void BSPTree::add_polygon(const Polygon &polygon)
{
	// just a wrapper for the private helper function
	_add_polygon_to_subtree(_root, polygon);
}

void BSPTree::clear()
{
	_clear_subtree(_root);

	// now the root node has been released, allocate a new, empty one for it
	// it is marked as 'out', as usual
	_root = new BSPNode(true);
}

void BSPTree::traverse(const Point &pov,
		BSPTree::InorderCallback callback, void *user_data)
{
	// just a wrapper for the private helper function
	_visit_subtree_inorder(_root, pov, callback, user_data);
}

void BSPTree::filter_polygon(const Polygon &polygon,
		BSPTree::InorderCallback callback, void *user_data)
{
	_filter_subtree(_root, polygon, callback, user_data);
}

void BSPTree::get_polygons(vector<Polygon> &list)
{
	// just a wrapper for the private helper function
	_collect_polygons(_root, list);
}

BSPTree::PositionType BSPTree::get_position(const Polygon &polygon) const
{
	// just another wrapper of a private helper function
	return _check_position(_root, polygon);
}

BSPTree::PositionType BSPTree::get_position(const Point &point) const
{
	return _check_position(_root, point);
}

///////////////////////////////////////////////////////////////////////////////

void BSPTree::_add_polygon_to_subtree(BSPNode *root, const Polygon &polygon)
{
	// first check if the polygon should be added to the root of the subtree
	if (root->is_leaf()) {
		// the root node is empty, just add the polygon to it
		root->convert(polygon);
	} else {
		// partitioning plane
		Polygon plane = root->plane();

		if (Utility::coplanar(polygon, plane)) {
			// the polygon is on the plane of the root node
			root->add_polygon(polygon);
		} else if (Utility::before(polygon, plane)) {
			// then decide which subspace it belongs
			// the polygon is in front of the partitioning plane
			// so it goes to the 'front' subtree
			_add_polygon_to_subtree(root->front(), polygon);
		} else if (Utility::behind(polygon, plane)) {
			// the polygon is behind the partitioning plane
			// so it goest to the 'back' subtree
			_add_polygon_to_subtree(root->back(), polygon);
		} else {
			// split the polygon if it spans the partition plane
			vector<Polygon> front_set;
			vector<Polygon> back_set;

			Utility::split_polygon(polygon, plane, front_set, back_set);

			// add fragments to the front subtree
			vector<Polygon>::iterator iter_front = front_set.begin();
			while (iter_front != front_set.end()) {
				_add_polygon_to_subtree(root->front(), *iter_front);
				iter_front++;
			}

			// add fragments to the back subtree
			vector<Polygon>::iterator iter_back = back_set.begin();
			while (iter_back != back_set.end()) {
				_add_polygon_to_subtree(root->back(), *iter_back);
				iter_back++;
			}
		}
	}
}

void BSPTree::_clear_subtree(BSPNode *root)
{
	if (root != NULL) {
		// clear the two subtrees first
		_clear_subtree(root->front());
		_clear_subtree(root->back());

		// then release the root of the subtree itself
		delete root;
	}
}

void BSPTree::_visit_subtree_inorder(BSPNode *root, const Point &pov,
		BSPTree::InorderCallback callback, void *user_data)
{
	if (root->is_leaf()) {
		// the current node is a leaf node, just invoke the callback
		_invoke_callback(root, pov, callback, user_data);
	} else {
		// obtain the partitioning plane of the root
		Polygon plane = root->plane();

		// always visit the subspace that is nearer to the pov
		float det = Utility::coplanar_det(plane, pov);
		if (Utility::is_zero(det)) {
			// the pov is on the plane
			// so just vist the 'front' and the 'back' subspace, and skip the
			// polygons on the plane
			_visit_subtree_inorder(root->front(), pov, callback, user_data);
			_visit_subtree_inorder(root->back(), pov, callback, user_data);
		} else if (Utility::before(pov, plane)) {
			// the pov is in front of the plane
			// so visit the 'front' subspace first
			_visit_subtree_inorder(root->front(), pov, callback, user_data);

			// now invoke the callback to process the polygons on the plane
			_invoke_callback(root, pov, callback, user_data);

			// visit the 'back' subspace at last
			_visit_subtree_inorder(root->back(), pov, callback, user_data);
		} else {
			// the pov is behind the plane
			// so visit the 'back' subspace first
			_visit_subtree_inorder(root->back(), pov, callback, user_data);

			// now invoke the callback to process the polygons on the plane
			_invoke_callback(root, pov, callback, user_data);

			// visit the 'front' subspace at last
			_visit_subtree_inorder(root->front(), pov, callback, user_data);
		}
	}
}

void BSPTree::_filter_subtree(BSPNode *root, const Polygon &polygon,
			BSPTree::InorderCallback callback, void *user_data)
{
	if (root->is_leaf()) {
		_invoke_callback(root, polygon, callback, user_data);
	} else {
		Polygon plane = root->plane();
		
		if (Utility::before(polygon, plane)) {
			_filter_subtree(root->front(), polygon, callback, user_data);
		} else if (Utility::behind(polygon, plane)) {
			_filter_subtree(root->back(), polygon, callback, user_data);
		} else if (Utility::coplanar(polygon, plane)) {
			return;
		} else {
			// split the polygon if it spans the partition plane
			vector<Polygon> front_set;
			vector<Polygon> back_set;

			Utility::split_polygon(polygon, plane, front_set, back_set);

			// add fragments to the front subtree
			vector<Polygon>::iterator iter_front = front_set.begin();
			while (iter_front != front_set.end()) {
				_filter_subtree(root->front(), *iter_front, callback, user_data);
				iter_front++;
			}

			// add fragments to the back subtree
			vector<Polygon>::iterator iter_back = back_set.begin();
			while (iter_back != back_set.end()) {
				_filter_subtree(root->back(), *iter_back, callback, user_data);
				iter_back++;
			}
		}
	}
}

void BSPTree::_invoke_callback(BSPNode *node, const Point &pov,
		BSPTree::InorderCallback callback, void *user_data)
{
	if (node->is_leaf()) {
		CallbackContext ctx(pov, node->plane(),
				Polygon(Point(0.0, 0.0, 0.0), Point(0.0, 0.0, 0.0), 
					Point(0.0, 0.0, 0.0)),
				true, true, node->out());

		// set the custom user data
		ctx.user_data = user_data;

		(*callback)(&ctx);
	} else {
		bool is_before = Utility::before(pov, node->plane());

		// iterate through all polygons on the plane
		vector<Polygon>::iterator iter = node->polygons().begin();
		while (iter != node->polygons().end()) {
			// create a callback context
			CallbackContext ctx(pov, node->plane(), (*iter),
					false, is_before, true);

			// set the custom user data
			ctx.user_data = user_data;

			// invode the callback
			(*callback)(&ctx);

			// move to next polygon
			++iter;
		}
	}
}
	
void BSPTree::_invoke_callback(BSPNode *node, const Polygon &polygon,
			BSPTree::InorderCallback callback, void *user_data)
{
	// dummy point of view
	Point pov(0.0, 0.0, 0.0);

	// node is always a leaf
	CallbackContext ctx(pov, node->plane(), polygon, true, true, node->out());

	// set the custom user data
	ctx.user_data = user_data;

	(*callback)(&ctx);
}

void BSPTree::_collect_polygons(BSPNode *root, vector<Polygon> &list)
{
	if (root != NULL) {
		// append the polygons on the current root node to the list
		vector<Polygon>::iterator iter = root->polygons().begin();
		while (iter != root->polygons().end()) {
			list.push_back(*iter);
			++iter;
		}

		// visit the two subtrees
		_collect_polygons(root->front(), list);
		_collect_polygons(root->back(), list);
	}
}

BSPTree::PositionType BSPTree::_check_position(BSPNode *root, 
		const Polygon &polygon) const
{
	if (root->is_leaf()) {
		if (root->out())
			return OUTSIDE;
		return INSIDE;
	} else {
		Polygon plane = root->plane();

		if (Utility::coplanar(polygon, plane)) {
			return ON_BOUNDARY;
		} else if (Utility::before(polygon, plane)) {
			return _check_position(root->front(), polygon);
		} else if (Utility::behind(polygon, plane)) {
			return _check_position(root->back(), polygon);
		} 
	}

	return SPAN;
}

BSPTree::PositionType BSPTree::_check_position(BSPNode *root,
		const Point &point) const
{
	if (root->is_leaf()) {
		if (root->out())
			return OUTSIDE;
		return INSIDE;
	} else {
		Polygon plane = root->plane();

		if (Utility::before(point, plane)) {
			return _check_position(root->front(), point);
		} else if (Utility::behind(point, plane)) {
			return _check_position(root->back(), point);
		}
	}

	return ON_BOUNDARY;
}
