//Christopher Needham and Robin Wu

#define MAKEFILE

#include <iostream>
#include "BTree.h"
#include "BTreeNode.h"
#include "LeafNode.h"
#include "InternalNode.h"

#ifdef NOMAKEFILE
#include "InternalNode.cpp"
#include "LeafNode.cpp"
#endif

#define NOFINALDEBUG

#define MAXNUMNODES 1000		//Defined because that's the maximum number of nodes this use case is expected to handle.

using namespace std;

BTree::BTree(int ISize, int LSize):internalSize(ISize), leafSize(LSize) {
	root = new LeafNode(LSize, NULL, NULL, NULL);

#ifdef FINALDEBUG
	cout << "Original root at:  " << root << endl;
#endif

} // BTree::BTree()

void BTree::insert(int value) {

	//make the insertion, but retain the return value so that I can screw with the root from here, rather than dealing with making the InternalNodes autodetect it.
	BTreeNode* rootReturn = root->insert(value);

	if (rootReturn) {	//If we got a return value back, then we know that the root node needs to split!

		//So first we allocate a new root node
		InternalNode* temp = new InternalNode(internalSize, leafSize, NULL, NULL, NULL);
#ifdef FINALDEBUG
		cout << "Root split:  " << temp << endl;
#endif

		//Then we put the old root and the new node into it
		temp->insertChild(rootReturn);	//Stick the new node in it
		temp->insertChild(root);	//temp is now officially the root

		root = temp;		//Update the class
	}
	return;
}

void BTree::print() {
	BTreeNode *BTreeNodePtr;
	Queue<BTreeNode*> queue(MAXNUMNODES);	//Interestingly, the only place this came into play.  IRL, probably use a queue based on a vector or something so that a maximum need not be defined.

	queue.enqueue(root);
	while(!queue.isEmpty())
	{
		BTreeNodePtr = queue.dequeue();
		BTreeNodePtr->print(queue);
	} // while
} // BTree::print()


