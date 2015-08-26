//Christopher Needham and Robin Wu

#ifndef LeafNodeH
#define LeafNodeH

#include "BTreeNode.h"
//When using makefiles, may need to change these .cpp's back to .h's, since the Makefile compiles each individually and then links them later

class LeafNode:public BTreeNode
{
	private:
		int *values;
		void insertValue(int value);
		int arrangeLarger(int value, bool goRight);
	public:
		LeafNode(int LSize, InternalNode *p, BTreeNode *left, BTreeNode *right);
		~LeafNode();
		int getMinimum() const;
		LeafNode* insert(int value); // returns pointer to new Leaf if splits, else NULL
		void print(Queue <BTreeNode*> &queue);	//Virtual function to satisfy BTreeNode class's requirments
}; //LeafNode class

#endif
