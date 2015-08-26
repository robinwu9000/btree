//Christopher Needham and Robin Wu

#ifndef InternalNodeH
#define InternalNodeH

#include "BTreeNode.h"

class InternalNode:public BTreeNode
{
	private:
		int internalSize;	//¹
		BTreeNode **children;
		int *keys;
	public:
		InternalNode(int ISize, int LSize, InternalNode *p,
				BTreeNode *left, BTreeNode *right);
		int getMinimum()const;
		InternalNode* insert(int value); // returns pointer to new InternalNode
		// if it splits else NULL
		//void insert(/*BTreeNode *oldRoot, BTreeNode *node2*/); // if root splits use this
		InternalNode* insert(BTreeNode *newChild); // from a sibling <-- function overloading was shitting itself in the worst kind of way
		void print(Queue <BTreeNode*> &queue);
		void updateMinimums();
		void insertChild(BTreeNode* newNode);
		BTreeNode* arrangeLarger(BTreeNode* extraNode, bool goRight);

}; // InternalNode class

#endif

/* Design notes
 * ¹	Remember that internalSize is the number of children pointers and the number of keys, since the leftmost key is the minimum of the entire node, to be reurned by getMinimum()
 *
 *
 *
 */
