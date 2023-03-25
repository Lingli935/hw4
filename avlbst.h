#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);
    int8_t computeBalance() const {
        int8_t balance = 0;
        if (getLeft()) {
            balance += getLeft()->balance_;
        }
        if (getRight()) {
            balance -= getRight()->balance_;
        }
        return balance;
    }

    void reComputeBalance() {
        setBalance(computeBalance());
    }
    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    AVLNode<Key, Value>* rightRotate(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* leftRotate(AVLNode<Key, Value>* node);
    void insertFix(AVLNode<Key, Value> *parent, AVLNode<Key, Value>* child);
    AVLNode<Key, Value>* getSuccessor(AVLNode<Key, Value>* node);
    void removeFix(AVLNode<Key, Value> *n, int diff);

		
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    AVLNode<Key,Value>* newNode = new AVLNode<Key,Value>(new_item.first, new_item.second, NULL);
    newNode->setBalance(0);   
    newNode->setRight(NULL);
    newNode->setLeft(NULL);
    
		if (this->root_ == NULL) {
        this->root_ = newNode;
        return;
    }
    AVLNode<Key,Value> *parent = NULL;
    AVLNode<Key,Value>* next = static_cast<AVLNode<Key,Value>*>(this->root_);
    while (true){
        parent = next;
        if (new_item.first  == parent->getKey()){
            parent->setValue(new_item.second);
            return;
        }
    		else if (new_item.first < parent->getKey()) {
            if (parent->getLeft() == NULL) {
                parent->setLeft(newNode);
                newNode->setParent(parent);
                break;
            }
            next = parent->getLeft();
        } 
        else {
            if (parent->getRight() == NULL) {
                parent->setRight(newNode);
                newNode->setParent(parent);
                break;
            }
            next = parent->getRight();
        }
    }
		if (parent->getBalance() == -1 || parent->getBalance() == 1) {
        parent->setBalance(0);
        return;
    }
    else {
        if (parent->getLeft() == newNode){
            parent->setBalance(-1);
        }
        else {
            parent->setBalance(1);
        }
        insertFix(parent, newNode);
    }

}
		

template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
		AVLNode<Key, Value>* node = static_cast<AVLNode<Key,Value>*>(this->internalFind(key));

    if (node == NULL) {
        return;  // empty tree
    }

    if (node->getLeft() != NULL && node->getRight() != NULL) {
        AVLNode<Key, Value>* predecessor = static_cast<AVLNode<Key,Value>*>(BinarySearchTree<Key, Value>::predecessor(node));
        nodeSwap(node, predecessor);
    }

    AVLNode<Key, Value> *child = node->getLeft();
    if (node->getRight() != NULL) {
        child = node->getRight();
    }

    AVLNode<Key, Value>* parent = node->getParent();
    if (child != NULL){
        child->setParent(parent);
    }

    int diff = 0;
    if (parent == NULL) {
        this->root_ = child;
    } 
    else {
        if (node == parent->getLeft()) {
            parent->setLeft(child);
            diff = 1;
        } 
        else {
            parent->setRight(child);
            diff = -1;
        }
    }

    // delete node
    delete node;

    removeFix(parent, diff);
 
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
 if((n1 == n2) || (n1 == NULL) || (n2 == NULL) ) {
        return;
    }
    Node<Key, Value>* n1p = n1->getParent();
    Node<Key, Value>* n1r = n1->getRight();
    Node<Key, Value>* n1lt = n1->getLeft();
    bool n1isLeft = false;
    if(n1p != NULL && (n1 == n1p->getLeft())) n1isLeft = true;
    Node<Key, Value>* n2p = n2->getParent();
    Node<Key, Value>* n2r = n2->getRight();
    Node<Key, Value>* n2lt = n2->getLeft();
    bool n2isLeft = false;
    if(n2p != NULL && (n2 == n2p->getLeft())) n2isLeft = true;


    Node<Key, Value>* temp;
    temp = n1->getParent();
    n1->setParent(n2->getParent());
    n2->setParent(temp);

    temp = n1->getLeft();
    n1->setLeft(n2->getLeft());
    n2->setLeft(temp);

    temp = n1->getRight();
    n1->setRight(n2->getRight());
    n2->setRight(temp);

    char temp2 = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(temp2);

    if( (n1r != NULL && n1r == n2) ) {
        n2->setRight(n1);
        n1->setParent(n2);
    }
    else if( n2r != NULL && n2r == n1) {
        n1->setRight(n2);
        n2->setParent(n1);

    }
    else if( n1lt != NULL && n1lt == n2) {
        n2->setLeft(n1);
        n1->setParent(n2);

    }
    else if( n2lt != NULL && n2lt == n1) {
        n1->setLeft(n2);
        n2->setParent(n1);

    }


    if(n1p != NULL && n1p != n2) {
        if(n1isLeft) n1p->setLeft(n2);
        else n1p->setRight(n2);
    }
    if(n1r != NULL && n1r != n2) {
        n1r->setParent(n2);
    }
    if(n1lt != NULL && n1lt != n2) {
        n1lt->setParent(n2);
    }

    if(n2p != NULL && n2p != n1) {
        if(n2isLeft) n2p->setLeft(n1);
        else n2p->setRight(n1);
    }
    if(n2r != NULL && n2r != n1) {
        n2r->setParent(n1);
    }
    if(n2lt != NULL && n2lt != n1) {
        n2lt->setParent(n1);
    }


    if(this->root_ == n1) {
        this->root_ = n2;
    }
    else if(this->root_ == n2) {
        this->root_ = n1;
    }
}



template<typename Key, typename Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value> *parent, AVLNode<Key, Value>* child)
 {
    // parent and grandparent should not be NULL
    if (parent == NULL || parent->getParent() == NULL) {
        return;
    }

    AVLNode<Key, Value> *grandparent = parent->getParent();

    if (parent == grandparent->getLeft()) { // left child of grandparent
        grandparent->setBalance(grandparent->getBalance() - 1);

        if (grandparent->getBalance() == 0) {
            return; //already balanced
        }

        if (grandparent->getBalance() == -1) {
            insertFix(grandparent, parent);
						//grandparent->reComputeBalance()
            return;
        }

        // grandparent balance is -2
        if (child == parent->getLeft()) {
            rightRotate(grandparent);
            parent->setBalance(0);
            grandparent->setBalance(0);

        } 
        else {
            leftRotate(parent);
            rightRotate(grandparent);

            if (child->getBalance() == -1) {
                parent->setBalance(0);
                grandparent->setBalance(1);

            } else if (child->getBalance() == 0) {
                parent->setBalance(0);
                grandparent->setBalance(0);

            } else {
                parent->setBalance(-1);
                grandparent->setBalance(0);
            }
            child->setBalance(0);
        }

    } 
    else { // right child of grandparent
        grandparent->setBalance(grandparent->getBalance() + 1);

        if (grandparent->getBalance() == 0) {
            return; // nothing to do, it is already balanced
        }

        if (grandparent->getBalance() == 1) {
            insertFix(grandparent, parent);
						//grandparent->reComputeBalance()
            return;
        }

        // grandparent balance is 2
        if (child == parent->getRight()) { // zig-zig - only 1 rotation
            leftRotate(grandparent);
            parent->setBalance(0);
            grandparent->setBalance(0);

        } 
        else { // zig-zag - 2 rotations
            rightRotate(parent);
            leftRotate(grandparent);

            if (child->getBalance() == 1) {
                parent->setBalance(0);
                grandparent->setBalance(-1);
            } 
            else if (child->getBalance() == 0) {
                parent->setBalance(0);
                grandparent->setBalance(0);

            } 
            else {
                parent->setBalance(1);
                grandparent->setBalance(0);
            }
            child->setBalance(0);
        }
    }
}


template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rightRotate(AVLNode<Key, Value>* x) {
 
 AVLNode<Key, Value>* y = x->getLeft();
    AVLNode<Key, Value>* rootParent = x->getParent();

    y->setParent(rootParent);
    if (x->getParent() == NULL) {        
        this->root_ = y;
    }
    else if (rootParent->getRight() == x){
        rootParent->setRight(y);
    }
    else{
        rootParent->setLeft(y);
    }    

    AVLNode<Key, Value>* c = y->getRight();

    y->setRight(x);
    x->setParent(y);
    x->setLeft(c);
    if (c != NULL){
        c->setParent(x);
    }

}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::leftRotate(AVLNode<Key, Value>* x) {

    AVLNode<Key, Value>* y = x->getRight();
    AVLNode<Key, Value>* rootParent = x->getParent();
    y->setParent(rootParent);

    //set the root parent
    if (x->getParent() == NULL) {        
        this->root_ = x->getRight();
    }
    else if (rootParent->getRight() == x){
        rootParent->setRight(y);
    }
    else{
        rootParent->setLeft(y);
    }    

    //pointer shifts
    AVLNode<Key, Value>* c = y->getLeft();

    y->setLeft(x);
    x->setParent(y);
    x->setRight(c);
    if (c != NULL){
        c->setParent(x);
    }

}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::getSuccessor(AVLNode<Key, Value>* node) 
{
    if (node->getRight() != NULL) {
        node = node->getRight();
        while (node->getLeft() != NULL) {
            node = node->getLeft();
        }
        return node;
    }
    else{
        AVLNode<Key, Value>* parent = node->getParent();
        while(parent != NULL && node == parent->getRight()){
            node = parent;
            parent = parent->getParent();
        }
        return parent;
    }
}
//print the tree 
//before removefix is called , see if the tree is rotating when should be rotated
template<typename Key, typename Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int diff)
{
    if (n == NULL){
        return;
    }

    AVLNode<Key, Value>* p = n->getParent();
    AVLNode<Key, Value>* c;

    int ndiff = 0;
    if (p != NULL && n==p->getLeft()){
        ndiff = 1;
    }

	 if (p != NULL && n==p->getRight()){
        ndiff = -1;
    }
    //case 1: b(n) + diff == -2
    if (n->getBalance() + diff == -2){
        //zig zig
        c = n->getLeft();
        if (c->getBalance() == -1 || c->getBalance() == 0){
            rightRotate(n);
            if(c->getBalance() == -1){
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p,ndiff);
            }
            if(c->getBalance() == 0){
                n->setBalance(-1);
                c->setBalance(1);
                return;
            }
        }
        else if (c->getBalance() == 1){
            AVLNode<Key, Value>* g = c->getRight();
            leftRotate(c);
            rightRotate(n);
            if (g->getBalance() == 1){
                n->setBalance(0);
                c->setBalance(-1);
                g->setBalance(0);
            }
             if(g->getBalance() == 0){
                n->setBalance(0);
                c->setBalance(0);
                g->setBalance(0);
            }
             if (g->getBalance() == -1){
                n->setBalance(1);
                c->setBalance(0);
                g->setBalance(0);
            }
            removeFix(p,ndiff);
        }
    }

 //case 1: b(n) + diff == 2
    else if (n->getBalance() + diff == 2){
        //zig zig
        c = n->getRight();
        if (c->getBalance() == 1 || c->getBalance() == 0){
            leftRotate(n);
            if(c->getBalance() == 1){
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p,ndiff);
            }
            if(c->getBalance() == 0){
                n->setBalance(1);
                c->setBalance(-1);
                return;
            }
        }
        else if (c->getBalance() == -1){
            AVLNode<Key, Value>* g = c->getLeft();
            rightRotate(c);
            leftRotate(n);
            if (g->getBalance() == -1){
                n->setBalance(0);
                c->setBalance(1);
                g->setBalance(0);
            }
             if(g->getBalance() == 0){
                n->setBalance(0);
                c->setBalance(0);
                g->setBalance(0);
            }
             if (g->getBalance() == 1){
                n->setBalance(-1);
                c->setBalance(0);
                g->setBalance(0);
            }
            removeFix(p,ndiff);
        }
    }

    // case for b(n) + diff == 2
    if (n->getBalance() + diff == 2){
        //zig zig
        c = n->getRight();
        if (c->getBalance() == 1 || c->getBalance() == 0){
            leftRotate(n);
            if(c->getBalance() == 1){
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p,ndiff);
            }
           if(c->getBalance() == 0){
                n->setBalance(1);
                c->setBalance(-1);
                return;
            }
        }
        else{//zig zag
            AVLNode<Key, Value>* g = c->getLeft();
            rightRotate(c);
            leftRotate(n);            
            if (g->getBalance() == -1){
                n->setBalance(0);
                c->setBalance(1);
                g->setBalance(0);
            }
             if(g->getBalance() == 0){
                n->setBalance(0);
                c->setBalance(0); 
                g->setBalance(0);

            }
             if (g->getBalance() == 1){
                n->setBalance(-1);
                c->setBalance(0);
                g->setBalance(0);
								}
              removeFix(p,ndiff);	
        }
    }


//case 2 b(n) + diff == -1
	else	if(n->getBalance() + diff == -1)
		{
			n->setBalance(-1);
			return;
		}

//case 3 b(n) + diff == 0
	else	if(n->getBalance() + diff == 0)
		{
			n->setBalance(0);
			removeFix(p,ndiff);
		}
}


#endif
