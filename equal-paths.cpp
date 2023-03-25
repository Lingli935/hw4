#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include  <iostream>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

bool traverse(const Node* node, int length, int& max_length) {
    if (!node) return true;
    if (!node->left && !node->right)
    {
        if (max_length == -1) {
            max_length = length;
        }
        return length == max_length;
    }
    return traverse(node->left, length + 1, max_length) && traverse(node->right, length + 1, max_length);
}

bool equalPaths(Node * root)
{
    // Add your code below
    int max_length = -1;
    return traverse(root, 0, max_length);
}
