//Christopher Needham and Robin Wu

#include <iostream>
#include "LeafNode.h"
#include "InternalNode.h"
#include "QueueAr.h"

using namespace std;

#define NOPRINTXS
#define NODEBUG

#define NOFINALDEBUG

LeafNode::LeafNode(int LSize, InternalNode *p, BTreeNode *left, BTreeNode *right) : BTreeNode(LSize, p, left, right)
{
	values = new int[LSize];
}  // LeafNode()

LeafNode::~LeafNode() {;}	//Blank for the moment, don't think there is anything.

int LeafNode::getMinimum() const {
	if(count > 0) {  // should always be the case
		return values[0];
	}
	else {
		throw "EMPTY LEAF";
	}
} // LeafNode::getMinimum()

LeafNode* LeafNode::insert(int value) {		//Returns null unless action is required on the part of the parent, in which case it returns the new node which needs a home
#ifdef DEBUG
	cout << "Inserting " << value << " into leaf @ " << this << ": ";
#endif
	//Insert into this node
	if (count < leafSize) {
#ifdef DEBUG
		cout << "Have found room in this node, inserting into values[]:\n";
#endif
		insertValue(value);
		if (parent) {
			parent->updateMinimums();
		}
		return NULL;
	}

	//Insert into sibling node
	else if ( ((rightSibling) && (rightSibling->getCount() < leafSize)) ||
			((leftSibling) && (leftSibling->getCount() < leafSize)) ) { //Check right sibling then left sibling


		//so the end result here is that value now contains the value to be sent to the new node.

		if ((leftSibling) && (leftSibling->getCount() < leafSize)) {
#ifdef DEBUG
			cout << " Going left ";
#endif
			//decide which value is getting kicked out
			value = arrangeLarger(value, false);
			leftSibling->insert(value);
		}

		//kick it out
		else { // ((rightSibling) && (rightSibling->getCount() < leafSize)) {
#ifdef DEBUG
			cout << " Going right ";
#endif
			//decide which value is getting kicked out
			value = arrangeLarger(value, true);
			rightSibling->insert(value);
		}

		if (parent) {
			parent->updateMinimums();
		}
		return NULL;
	}

	//Insert into
	else { //We cannot find space and must split
#ifdef DEBUG
		cout << "Need to make a new leaf node: ";
#endif
		//Decide which to kick out and rearrange data array
		value = arrangeLarger(value, true);

		//Create new node
		LeafNode* newnode = new LeafNode(leafSize, parent, this, rightSibling);	//Make new node, considerations at ¹.  Has only changed parameters of newnode.
#ifdef FINALDEBUG
		cout << "New child LeafNode:  " << newnode << endl;
#endif
		if (rightSibling) {		//If we have a right sibling
			rightSibling->setLeftSibling(newnode);		//Set the left sibling of our soon-to-be-former right sibling to the new node
		}

		this->rightSibling = newnode;

		//Copy stuff to new node
		newnode->insert(value);			//rightSibling.insert(value) should be an equivalent call
		for (int pos=leafSize-1; pos>((leafSize/2)-1+(leafSize%2)); --pos) {
#ifdef DEBUG
			cout << "Inserting " << values[pos] << endl;
#endif
			newnode->insert(values[pos]);
			values[pos] = 0;						//²
			--count;
		}

		if (parent) {
			parent->updateMinimums();
		}

		return newnode;
	}
}  // LeafNode::insert()

void LeafNode::print(Queue <BTreeNode*> &queue) {	//Queue needs to be passed in to satisfy 'virtual' requirement.
	cout << "Leaf: ";
	for (int i = 0; i < leafSize; i++) {
		if (i < count) {
			cout << values[i] << ' ';
		}
		else {
#ifdef PRINTXS
			cout << 'X' << ' ';
#endif
#ifndef PRINTXS
			//cout << ' ' << ' ';
#endif
		}
	}
	cout << endl;
} // LeafNode::print()

void LeafNode::insertValue(int value) {
	for (int pos=count; pos>=0; --pos) {	//count = 1 + highest values array index in use
		if ((pos>0) && (value < values[pos-1])) {	//If the value to the left of the current position is smaller than the new value
#ifdef DEBUG
			cout << value << " < " << "values[" << pos-1 << "]";
#endif
			values[pos]=values[pos-1];	//We need to move left, so we'll scootch the value over to the right while we do.
		}
		else {
#ifdef DEBUG
			cout << "Inserting at values[" << pos << "], count = " << count << endl;
#endif
			values[pos] = value;
			++count;
			break;			//If we don't break out of the loop here, we overwrite the rest of the values with the newly-inserted value
		}
	}
	return;
}

int LeafNode::arrangeLarger(int value, bool goRight) {		//Ensures that the largest value in in 'value' (returning this value) and that the values[] is in order
	if ((goRight) && (value < values[leafSize-1])) {		//If the value we're inserting is not the max value,
#ifdef DEBUG
		cout << "Kicking the previous maximum, " << values[leafSize-1];
#endif
		int temp = value;			//Exchange it with the end of the array,
		value = values[leafSize-1];
		values[leafSize-1] = temp;

		--count;				//decrement count,
		insertValue(values[leafSize-1]);	//re-insert the value to make sure it goes into the correct spot.
	}
	else if ((!goRight) && (value > values[0])) {		//If the value we're inserting is not the max value,
#ifdef DEBUG
		cout << "Kicking the previous minimum, " << values[leafSize-1];
#endif
		int temp = value;			//Exchange it with the end of the array,
		value = values[0];
		values[0] = temp;

		//temp = values[0];			//Duplicate instruction, not needed
		for (int i=0; i<(leafSize-1); i++) {
			values[i]=values[i+1];
		}
		values[leafSize-1]=temp;

		--count;				//decrement count,
		insertValue(values[leafSize-1]);	//re-insert the value to make sure it goes into the correct spot.
	}


#ifdef DEBUG
	else {
		cout << "Kicking out the new value, " << value;
	}
#endif
	return value;
}

/* Design notes
 * ¹	Because of how the B⁺ structure works, and the assumption that we will
 			always create a right node in this routine, we know that the left node
			will be the creating node, and the right sibling will be the right sibling
			of the creating node.  Similarly, we can update the creating node's right
			sibling to be the node which we just created.  The parent can be one of
			two parents, either the creating node's parent or the right sibling to
			that parent, but the determination would require a ton of function calls
			to determine from the position of the creating node, and will be left to
			the parent.  So the parent point is left NULL, to be filled by the parent
			as this node's position is decided.
 *
 * ²	Technically this probably is not necessary since these values should never be compared due to the --count below it, but I'm going to keep it in place for debugging confirmation.
 *
 */
