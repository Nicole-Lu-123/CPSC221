#include "chain.h"
#include "chain_given.cpp"
#include <iostream>
using namespace std;

// PA1 functions

/**
 * Destroys the current Chain. This function should ensure that
 * memory does not leak on destruction of a chain.
 */

Chain::~Chain()
{
  clear();
  delete head_;
}

/**
 * Inserts a new node at the end of the List.
 * This function **SHOULD** create a new ListNode.
 *
 * @param ndata The data to be inserted.
 */
void Chain::insertBack(const Block &ndata){
  Node *nNode = new Node(ndata);
  nNode->prev = head_->prev;
  head_->prev->next = nNode;
  nNode->next = head_;
  head_->prev = nNode;

  length_++;
}

/**
 * Modifies the Chain by moving the subchain of len Nodes,
 * starting at position startPos, dist positions toward the
 * end of the chain.  If startPos + len - 1 + dist > length
 * then dist = length - startPos - len + 1 (to prevent gaps
 * in the new chain).
 * The subchain occupies positions (startPos + dist) through
 * (startPos + dist + len - 1) of the resulting chain.
 * The order of subchain nodes is not changed in the move.
 * You may assume that: 1 <= startPos <= length - len + 1,
 * 0 <= dist <= length, and 0 <= len <= length. 
 */
void Chain::moveBack(int startPos, int len, int dist)
{ 
  if (dist == 0)
  {
    return;
  }
  if (startPos + len - 1 + dist > length_)
  {
    dist = length_ - startPos - len + 1;
  }
 Node * firstNode = walk(head_, startPos);
	Node * lastNode = walk(head_, startPos+len-1);
	Node * afterNode = walk(head_, startPos+len+dist-1);
	Node * beforeNode = walk(head_, startPos+len+dist);
	lastNode->next->prev = firstNode->prev;
	firstNode->prev->next = lastNode->next;
	firstNode->prev = afterNode;
	afterNode->next = firstNode;
	lastNode->next = beforeNode;
	beforeNode->prev = lastNode;
 
}

/**
 * Rolls the current Chain by k nodes: reorders the current list
 * so that the first k nodes are the (n-k+1)th, (n-k+2)th, ... , nth
 * nodes of the original list followed by the 1st, 2nd, ..., (n-k)th
 * nodes of the original list where n is the length.
 */
void Chain::roll(int k){
	if (length_ == 0 || k == 0 || k == length_){
		return;
  }
  else
  {
    k = k % length_;
    Node *beginNode = walk(head_,length_-k+1);
    Node *endNode = walk(head_,length_);
    Node *endNewNode = walk(head_,length_-k);
    Node *afterendNode =walk(head_,1);

    head_->prev = endNewNode;
    endNewNode->next = head_;
    head_->next = beginNode;
    beginNode ->prev = head_;
    endNode->next = afterendNode;
    afterendNode->prev = endNode;
    
  }

}


/**
 * Modifies the current chain by reversing the sequence
 * of nodes from pos1 up to and including pos2. You may
 * assume that pos1 and pos2 exist in the given chain,
 * and pos1 <= pos2.
 * The positions are 1-based.
 */
void Chain::reverseSub(int pos1, int pos2){
  if(pos1 < pos2){
		Node * firstNode = walk(head_, pos1);
		Node * secondNode = walk(head_, pos2);
		if(pos1+1 == pos2){
			Node * beforeFirstNode = walk(head_, pos1-1);
			Node * afterSecondNode = walk(head_, pos2+1);
			beforeFirstNode->next = secondNode;
			secondNode->prev = beforeFirstNode;
			secondNode->next = firstNode;
			firstNode->prev = secondNode;
			firstNode->next = afterSecondNode;
			afterSecondNode->prev = firstNode;

		}
		else{
			Node * beforeFirstNode = walk(head_, pos1-1);
			Node * afterSecondNode = walk(head_, pos2+1);
			Node * afterFirstNode = walk(head_, pos1+1);
			Node * beforeSecondNode = walk(head_, pos2-1);
			beforeFirstNode->next = secondNode;
			secondNode->prev = beforeFirstNode;
			firstNode->prev = beforeSecondNode;
			beforeSecondNode->next = firstNode;
			firstNode->next = afterSecondNode;
			afterSecondNode->prev = firstNode;
			secondNode->next = afterFirstNode;
			afterFirstNode->prev = secondNode;
		}
		reverseSub(pos1+1,pos2-1);
	}
  
}

/*
* Modifies both the current chain and the "other" chain by removing
* nodes from the other chain and adding them between the nodes
* of the current chain -- one "other" node between two current nodes --
* until one of the two chains is exhausted.  Then the rest of the
* nodes in the non-empty chain follow.
* The length of the resulting chain should be the sum of the lengths
* of current and other. The other chain should have only
* the head_ sentinel at the end of the operation.
* The weave fails and the original
* chains should be unchanged if block sizes are different.
* In that case, the result of the function should be:
* cout << "Block sizes differ." << endl; leaves other empty.
*/
void Chain::weave(Chain &other)
{
  if (width_ != other.width_ || height_ != other.height_)
  {
    cout << "Block sizes differ." << endl;
  }
  else
  {
    int doubleLength;
    	bool s;

    	if(length_ < other.length_){
			doubleLength = 2*length_;
			s = true;
   		}
       else{
    		doubleLength = 2*other.length_;
    		s = false;
   		}

    for(int i = 1 ; i < doubleLength; i = i+2){
    

      Node *firstNode = walk(head_, i);
      Node *afterfirstNode = firstNode->next;

      Node *secondNode = walk(other.head_, 1);
      Node *aftersecondNode = secondNode->next;

      firstNode->next = secondNode;
      secondNode->prev = firstNode;
      secondNode->next = afterfirstNode;
      afterfirstNode->prev = secondNode;
      other.head_->next = aftersecondNode;
      aftersecondNode->prev = other.head_;

      length_++;
      other.length_--;
    }
    if (s)
    {
      Node *afterother = other.head_->next;
      Node *endother = other.head_->prev;
      Node *endhead = head_->prev;
      endhead->next = afterother;
      afterother->prev = endhead;
      head_->prev = endother;
      endother->next = head_;
      other.head_->prev = other.head_;
      other.head_->next = other.head_;

      length_ = length_ + other.length_;
     		other.length_ = 0;
    }
  }
  
}

/**
 * Destroys all dynamically allocated memory associated with the
 * current Chain class except for the sentinel head_. Sets length_
 * to zero.  After clear() the chain represents an empty chain.
 */
void Chain::clear()
{
  while (head_->next != head_)
  {
    Node *curr = head_->next;
    Node *next = curr->next;
    next->prev = head_;
    delete head_->next;
    head_->next = next;

    length_--;
  }
  height_ = 0;
  width_ = 0;
  
}


/**
 * Makes the current object into a copy of the parameter:
 * All member variables should have the same value as
 * those of other, but the memory should be completely
 * independent. This function is used in both the copy
 * constructor and the assignment operator for Chains.
 */
void Chain::copy(Chain const &other)
{
  height_ = other.height_;
  width_ = other.width_;

  head_ = new Node(Block());
  head_->next = head_;
  head_->prev = head_;

  length_ = 0;

  if (other.length_ != 0)
  {

    for (int i = 1; i <= other.length_; i++)
    {
      Block b;
      Node *curr = walk(other.head_, i);
      b = curr->data;
      insertBack(b);
    }
  }
 
}
