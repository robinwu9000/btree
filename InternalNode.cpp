//Christopher Needham and Robin Wu

#include <iostream>
#include "InternalNode.h"


using namespace std;

InternalNode::InternalNode(int ISize, int LSize,
		InternalNode *p, BTreeNode *left, BTreeNode *right) :
	BTreeNode(0, p, left, right), internalSize(ISize) //¹
{
	keys = new int[internalSize]; // keys[i] is the minimum of children[i]
	children = new BTreeNode* [ISize];

	for (int i=0; i<ISize; i++) {	//Ensure that all the addresses are truly blank for accurate debugging.
		children[i]=NULL;
		keys[i]=0;	//While we're here....
	}

} // InternalNode::InternalNode()

//InternalNode::~InternalNode() {;}	//²

int InternalNode::getMinimum() const {
	if(count > 0)   // should always be the case
		return children[0]->getMinimum();
	else
		return 0;
} // InternalNode::getMinimum()


InternalNode* InternalNode::insert(int value) {
	int pickchild=1;

	//Determine which child to insert into.
	while (pickchild<count) {
		if (value < keys[pickchild]) {
			--pickchild; //Needed to correct relative array offsets.
			break;
		}
		else {
			++pickchild;
		}
	}
	if (pickchild >= count) {
		pickchild = count-1;
	}

	//Call appropriate child -> insert(value) and redirect handling splitting

	BTreeNode* childSplit = (children[pickchild])->insert(value);
	if (childSplit) {
		InternalNode* weSplit = this->insert(childSplit);			//Personally, I like using 'this' because imhop it enhances readability.
		this->updateMinimums();							//Naturally, there's a question of when the readability can be helped.
		return weSplit;								//But when I'm working with multiple classes of the same type,
	}										//I find that 'this->' helps me remember which function I'm referencing
	else {										//By making what is naturally implicit explicit.
		this->updateMinimums();
		return 0;
	}	//Means that the child accepted the value without issue.

} // InternalNode::insert()

//void InternalNode::insert(/*BTreeNode *oldRoot, BTreeNode *node2*/) { // Will be called to split thhe root node

InternalNode* InternalNode::insert(BTreeNode *newChild) { // from a sibling or from self <--Had to change name since compiler was borking overloading and kept trying to convert a BTreeNode to an int -- no frickin' idea why, but whatever.
	//If the new node can fit in this node
	if (count < internalSize) {	//Then we have the room to stash a new node
		insertChild(newChild);
		this->updateMinimums();
		if (parent) {
			parent->updateMinimums();
		}
		return NULL;
	}

	else if ( ((rightSibling) && (rightSibling->getCount() < internalSize)) ||
		  ((leftSibling)  && (leftSibling->getCount()  < internalSize)) ) {


		if ((leftSibling) && (leftSibling->getCount() < internalSize)) {			//Piazza says check left sibling first, so we have to check left sibling first..
			newChild = arrangeLarger(newChild, false);
			newChild->setParent(dynamic_cast<InternalNode*>(leftSibling));
			(dynamic_cast<InternalNode*>(leftSibling))->insert(newChild);
		}



		else { // ((rightSibling) && (rightSibling->getCount() < internalSize)) {
			newChild = arrangeLarger(newChild, true);
			newChild->setParent(dynamic_cast<InternalNode*>(rightSibling));				//Set the future parent of the child before it's sent off
			(dynamic_cast<InternalNode*>(rightSibling))->insert(newChild);	//More boilerplate on the left in order to make sure the system knows where to find rightSibling
		}


		this->updateMinimums();
		if (parent) {
			parent->updateMinimums();
		}
		return NULL;
	}

	else { //if we need to split
		//Decide whcih value to kick and rearrange children array
		newChild = arrangeLarger(newChild, true);

		//Create a new node
		BTreeNode* newSibling = new InternalNode(internalSize, 0, parent, this, rightSibling);
		if (rightSibling) {
			rightSibling->setLeftSibling(newSibling);
		}
		this->rightSibling = newSibling;

		//Copy stuff to the new sibling
		(dynamic_cast<InternalNode*>(newSibling))->insert(newChild);
		for (int pos=internalSize-1; pos>((internalSize/2)-1+(internalSize%2)); --pos) {
			(dynamic_cast<InternalNode*>(newSibling))->insert(children[pos]);
			children[pos]=NULL;
			keys[pos] = 0;			//Zero out removed keys as well as removed children.
			--count;
		}
		this->updateMinimums();
		if (parent) {
			parent->updateMinimums();
		}
		return (dynamic_cast<InternalNode*>(newSibling));
	}
} // InternalNode::insert()

void InternalNode::print(Queue <BTreeNode*> &queue)
{
	int i;

	cout << "Internal: ";
	for (i = 0; i < count; i++)
		cout << keys[i] << ' ';
	cout << endl;

	for(i = 0; i < count; i++)
		queue.enqueue(children[i]);

} // InternalNode::print()

void InternalNode::updateMinimums() {
	for (int i=0; i<count; i++) {
		keys[i] = children[i]->getMinimum();
	}
	return;
}

void InternalNode::insertChild(BTreeNode* newNode) {
	int newMinimum = newNode->getMinimum();

	//This has probably been handled at other points in the code, but I will ensure that it's handled here
	newNode->setParent(this);

	for (int pos=count; pos>=0; --pos) {
		if ((pos>0) && (newMinimum < keys[pos-1])) {
			keys[pos] = keys[pos-1];	//Scootching values over to the right as we go
			children[pos] = children[pos-1];
		}

		else {
			keys[pos] = newMinimum;
			children[pos] = newNode;
			++count;
			break;
		}
	}

	this->updateMinimums();
	if (parent) {
		parent->updateMinimums();
	}

	return;
}

BTreeNode* InternalNode::arrangeLarger(BTreeNode* extraNode, bool goRight) {
	int extraMinimum = extraNode->getMinimum();

	BTreeNode* temp = extraNode;

	if ((goRight) && (extraMinimum < keys[internalSize-1])) {

		extraNode = children[internalSize-1];
		children[internalSize-1] = temp;
		keys[internalSize-1] = temp->getMinimum();	//ANY copies back into the children[] array require keys[] to be set appropriately!

		--count;
		insertChild(children[internalSize-1]);
	}

	else if ((!goRight) && (extraMinimum > keys[0])) {		//Generally, see LeafSize.cpp for comments on this function -- only a
									//few values were changed
		extraNode = children[0];
		children[0] = temp;
		keys[0] = temp->getMinimum();		//ANY copies back into children[] require keys[] to be updated as well!

		for (int i=0; i<(internalSize-1); i++) {
			children[i] = children[i+1];
			keys[i] = keys[i+1];
		}
		children[internalSize-1] = temp;
		keys[internalSize-1] = temp->getMinimum();

		--count;
		insertChild(children[internalSize-1]);
	}

	this->updateMinimums();
	if (parent) {
		parent->updateMinimums();
	}

	return extraNode;
}


/* Design Notes
 *
 * ¹	The leafSize value has been set to zero for internal nodes.  This is
 			because we need some way to differentiate between internal nodes and leaf
			nodes for various operations including insert() and find(), and  have
			decided that due to the need to differentiate for insert() anyway, the
			'main' insert() function will be in the BTree class and will not be
			recursive.  This will net us a speed gain and possibly simplified
			programming.  If leafSize is zero, then this node is internal.  If it's
			nonzero, then this is a leaf node.  This lends itself well to
 *
 * ²	Putting the generic destructor in here, flouting good coding practices,
 			and relying on the OS to properly reclaim memory after the program has
			exited.  This is because that mechanism will work just fine, and we're not
			handling deletes right now.  Additionally, were we handling deletes, a
			different kind of destruct-ing mechanism would be required, since deleting
		  one node is not the same as deleting a subtree.
 *
 */
