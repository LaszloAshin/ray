#include <iostream>
#include "KdTree.h"

using namespace std;

KdTree::Node::Node(unsigned n)
{
	left = right = NULL;
	depth = 0;
	no = n;
	objects = no ? new BaseObject*[no] : NULL;
	half = 0.0f;
}

KdTree::Node::~Node()
{
	if (objects != NULL)
		delete[] objects;
}

void
KdTree::Node::spread()
{
	cerr << "spread: " << no << ", " << depth << endl;
	if (no < 42) return;
	Vector mid;
	int n = 0;
	for (unsigned i = 0; i < no; ++i) {
		if (!objects[i]->hasMidPoint()) continue;
		mid = mid + objects[i]->midPoint();
		++n;
	}
	// TODO: we will need (better) heuristics for finding
	// the splitting hyperplane

	if (n < 42) return;
	mid = mid * (1.0f / n);

	int *status = new int[no];
	int nleft = 0, nright = 0;
	Axis axis = (Axis)(depth % 3);
	switch (axis) {
		case axisX:
			half = mid.x;
			break;
		case axisY:
			half = mid.y;
			break;
		case axisZ:
			half = mid.z;
			break;
	}

	cerr << axis << ", " << half << ": ";

	for (unsigned i = 0; i < no; ++i) {
		status[i] = objects[i]->onSideOfPlane(axis, half);
		cerr << status[i] << ", ";
		if (status[i] <= 0) ++nleft;
		if (status[i] >= 0) ++nright;
	}

	cerr << endl << "children left|right: " << nleft << '|' << nright << endl;
	left = new Node(nleft);
	right = new Node(nright);
	left->depth = right->depth = depth + 1;
	unsigned nl = 0, nr = 0;
	for (unsigned i = 0; i < no; ++i) {
		if (status[i] <= 0)
			left->objects[nl++] = objects[i];
		if (status[i] >= 0)
			right->objects[nr++] = objects[i];
	}

	delete[] status;
	delete[] objects;
	no = 0;
	left->spread();
	right->spread();
}

KdTree::KdTree() : firstObj(NULL)
{
	root = NULL;
	no = 0;
}

KdTree::~KdTree()
{
	freeNode(root);
	while (firstObj != NULL) {
		BaseObject *p = firstObj;
		firstObj = p->next;
		delete p;
	}
}

void
KdTree::freeNode(Node *n)
{
	cerr << "freeNode(" << hex << (unsigned long)n << ')' << endl;
	if (n == NULL) return;
	freeNode(n->left);
	freeNode(n->right);
	cerr << "freeing up it (" << hex << (unsigned long)n << ')' << endl;
	delete n;
	cerr << "freeing up succeeded." << endl;
}

void
KdTree::addObject(BaseObject *p)
{
	p->next = firstObj;
	firstObj = p;
	++no;
}

void
KdTree::build()
{
	cerr << "Building kd-tree... ";

	root = new Node(no);
	int i;
	BaseObject *o;
	for (i = 0, o = firstObj; o != NULL; o = o->next, ++i)
		root->objects[i] = o;

	root->spread();

	cerr << "done." << endl;
}

const KdTree::Node *
KdTree::getNode(const Vector& p) const
{
	int d = 0;
	Node *n = root;
	Node *ret = NULL;
	while (n != NULL) {
		ret = n;
		switch (d) {
			case 0:
				n = (p.x > n->half) ? n->right : n->left;
				break;
			case 1:
				n = (p.y > n->half) ? n->right : n->left;
				break;
			case 2:
				n = (p.z > n->half) ? n->right : n->left;
				break;
		}
		d = (d + 1) % 3;
	}
	return ret;
}

#ifndef USE_KD_TREE
float
KdTree::intersect(const Ray &r, Vector &N, BaseObject **O) const
{
	float t = 0.0f;
	bool found = false;
	getNode(r.s);
	for (BaseObject *p = firstObj; p != NULL; p = p->next) {
		Vector nv;
		float to = p->intersect(r, nv);
		if (to > 0.0f && (!found || to < t)) {
			found = true;
			t = to;
			N = nv;
			if (O != NULL) *O = p;
		}
	}
	return found ? t : -1.0f;
}
#else
float
KdTree::intersect(const Ray &r, Vector &N, BaseObject **O) const
{
	struct Item {
		Item *next;
		Node *node;

		Item(Node *n, Item *x = NULL)
		{
			next = x;
			node = n;
		}
	} *first;

	first = new Item(root);
	while (first != NULL) {
		Node *n = first->node;
		Item *item = first;
		first = item->next;
		delete item;

		// go deeper in the tree if we can
		if (n->left && n->right) {
			bool bo = false;
			bool in = false;
			switch ((Axis)(n->depth % 3)) {
				case axisX:
					bo = r.s.x > n->half;
					in = r.d.x > 0;
					break;
				case axisY:
					bo = r.s.y > n->half;
					in = r.d.y > 0;
					break;
				case axisZ:
					bo = r.s.z > n->half;
					in = r.d.z > 0;
					break;
			}

			if (bo) {
				if (!in) first = new Item(n->left, first);
				first = new Item(n->right, first);
			} else {
				if (in) first = new Item(n->right, first);
				first = new Item(n->left, first);
			}

			continue;
		}

		float t = 0.0f;
		bool found = false;
		for (unsigned i = 0; i < n->no; ++i) {
			Vector nv;
			float to = n->objects[i]->intersect(r, nv);
			if (to > 0.0f && (!found || to < t)) {
				Vector mp = r.s + r.d * to;
				if (getNode(mp) == n) {
					found = true;
					t = to;
					N = nv;
					if (O != NULL)
						*O = n->objects[i];
				}
			}
		}
		if (found) {
			while (first != NULL) {
				Item *item = first;
				first = item->next;
				delete item;
			}
			return t;
		}

	}
	return -1.0f;
}
#endif

void
KdTree::dumpSub(Node *n, int depth)
{
	if (!n) return;
	for (int i = depth; i; --i)
		cerr << ' ';
	cerr << "node: " << n->no << endl;
	dumpSub(n->left, depth + 1);
	dumpSub(n->right, depth + 1);
}
