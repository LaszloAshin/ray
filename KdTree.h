#ifndef _KDTREE_H
#define _KDTREE_H	1

enum Axis { axisX, axisY, axisZ };
class KdTree;

#include "BaseObject.h"

class KdTree {
	BaseObject *firstObj; // initial object list
	unsigned no; // number of objects in the list

	class Node {
		friend class KdTree;

		Node *left, *right;
		int depth;
		unsigned no; // number of objects in this node
		BaseObject **objects;
		float half; // splitting hyperplane - origo distance

		Node(unsigned n);
		~Node();
		void spread();
	} *root;

	void freeNode(Node *n);
	const Node *getNode(const Vector& p) const;
	void dumpSub(Node *n, int depth);

public:
	KdTree();
	~KdTree();

	void addObject(BaseObject *p);
	void build();
	float intersect(const Ray &r, Vector &N, BaseObject **O) const;
	void dump() { dumpSub(root, 0); }
};

#endif /* _KDTREE_H */
