// mymap.h
//
// TODO: write this file header comment.
#pragma once

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

template<typename keyType, typename valueType>
class mymap {
 private:
    struct NODE {
        keyType key;  // used to build BST
        valueType value;  // stored data for the map
        NODE* left;  // links to left child
        NODE* right;  // links to right child
        int nL;  // number of nodes in left subtree
        int nR;  // number of nodes in right subtree
        bool isThreaded;
    };
    NODE* root;  // pointer to root node of the BST
    int size;  // # of key/value pairs in the mymap

    //
    // iterator:
    // This iterator is used so that mymap will work with a foreach loop.
    //
    struct iterator {
     private:
        NODE* curr;  // points to current in-order node for begin/end

     public:
        iterator(NODE* node) {
            curr = node;
        }

        keyType operator *() {
            return curr -> key;
        }

        bool operator ==(const iterator& rhs) {
            return curr == rhs.curr;
        }

        bool operator !=(const iterator& rhs) {
            return curr != rhs.curr;
        }

        bool isDefault() {
            return !curr;
        }

        //
        // operator++:
        //
        // This function should advance curr to the next in-order node.
        // O(logN)
        //
        iterator operator++() {
            if (curr->isThreaded) {     // curr is a leaf node
                curr = curr->right;     // so next is its parent;
                return iterator(curr);
            }

            // otherwise, we're in the branches so next will be left-most node
            // of curr's right subtree;
            curr = curr->right;
            while (curr->left != nullptr) {
                curr = curr->left;
            }
            return iterator(curr);
        }
    };

    // _clear:
    //
    // Recursive helper function for clear()
    //
    void _clear(NODE* n) {
        if (n == nullptr)
            return;
        
        _clear(n->left);
        if (!n->isThreaded)
            _clear(n->right);
        delete n;
        size--;
    }

    //
    // _copy:
    //
    // Recursive helper function for copying one mymap into another. Used
    // mostly for equals operator and copy constructor
    //
    void _copy(NODE* n) {
        if (n == nullptr)
            return;
        else {
            put(n->key, n->value);
            _copy(n->left);
            if (!n->isThreaded)
                _copy(n->right);
        }
    }

    //
    // isBalanced:
    // Returns true if the node satisfies the balance condition, false otherwise
    //
    bool isBalanced(NODE* n) {
        return (n->nL >= n->nR) ? (n->nL <= (n->nR * 2) + 1) : (n->nR <= (n->nL * 2) + 1);
    }

    //
    // makeNode:
    // Returns node with provided key and value, child pointers set to null,
    // subtree sizes set to 0, and isThreaded = true
    //
    NODE* makeNode(keyType key, valueType value) {
        NODE* n = new NODE;
        n->key = key;
        n->value = value;
        n->left = nullptr;
        n->right = nullptr;
        n->nL = 0;
        n->nR = 0;
        n->isThreaded = true;

        return n;
    }

    //
    // fillVector:
    // Takes a tree with root n and fills a vector with nodes, in-order
    //
    void fillVector(NODE* n, vector<NODE*>& vec) {
        if (n == nullptr) {
            return;
        }

        fillVector(n->left, vec);

        vec.push_back(n);

        if (!n->isThreaded) {
            fillVector(n->right, vec);
        }
    }

    //
    // getNumChildren:
    // returns the size of a node's combined subtrees, or 0 if n is null
    //
    int getNumChildren(NODE* n) {
        return (n != nullptr) ? (n->nL + n->nR) : 0;
    }

    //
    // buildFromVector:
    // Recursive helper function for rebalance(), takes vector and bounds
    // and rebuilds tree from vector. The root of the new tree is the middle
    // element of the vector. Each subsequent call builds a child of the node
    // that is the middle element of one half of the vector.
    //
    NODE* buildFromVector(NODE* n, vector<NODE*> vec, int left, int right){
        // base case
        if (left > right) {
            return nullptr;
        }

        int mid = (left + right) / 2;
        n = vec[mid];

        n->nL = 0;
        n->nR = 0;
        n->isThreaded = true;

        // recursion
        n->left = buildFromVector(n->left, vec, left, mid-1);
        n->right = buildFromVector(n->right, vec, mid+1, right);

        n->nL = getNumChildren(n->left) + (n->left != nullptr);
        n->nR = getNumChildren(n->right) + (n->right != nullptr);

        if (n->right != nullptr) {
            n->isThreaded = false;
        }

        return n;
    }

    //
    // rebalance:
    //
    // returns pointer to root NODE of new, rebalanced subtree
    //
    NODE* rebalance(NODE* n) {
        vector<NODE*> vec;
        int left, right;

        fillVector(n, vec);
        left = 0;
        right = vec.size() - 1;

        // insert element at [mid]
        n = buildFromVector(n, vec, left, right);

        return n;
    }

    //
    // _toVector:
    // Recursive helper function to create a vector of key/value pairs, in-order
    //
    void _toVector(NODE* n, vector<pair<keyType, valueType>>& vec) {
        if (n == nullptr) {
            return;
        }

        _toVector(n->left, vec);

        pair<keyType, valueType> nodePair;
        nodePair = make_pair(n->key, n->value);
        vec.push_back(nodePair);

        if (!n->isThreaded) {
            _toVector(n->right, vec);
        }
    }

    //
    // _toString:
    // Recursive helper function for toString, recursively creates string of
    // map's contents
    //
    void _toString(NODE* n, stringstream& ss) {
        if (n == nullptr) {
            return;
        }

        _toString(n->left, ss);

        ss << "key: " << n->key << " value: " << n->value << "\n";

        if (!n->isThreaded) {
            _toString(n->right, ss);
        }
    }

    //
    // _checkBalance:
    // Recursive helper function for checkBalance
    //
    void _checkBalance(NODE* n, stringstream& ss) {
        if (n == nullptr) {
            return;
        }

        ss << "key: " << n->key << ", nL: " << n->nL << ", nR: " << n->nR << "\n";

        _checkBalance(n->left, ss);
        if (!n->isThreaded) {
            _checkBalance(n->right, ss);
        }
    }

 public:
    //
    // default constructor:
    //
    // Creates an empty mymap.
    // Time complexity: O(1)
    //
    mymap() {
        root = nullptr;
        size = 0;
    }

    //
    // copy constructor:
    //
    // Constructs a new mymap which is a copy of the "other" mymap.
    // Sets all member variables appropriately.
    // Time complexity: O(nlogn), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    mymap(const mymap& other) {
        root = nullptr;
        size = 0;
        _copy(other.root);
    }

    

    //
    // operator=:
    //
    // Clears "this" mymap and then makes a copy of the "other" mymap.
    // Sets all member variables appropriately.
    // Time complexity: O(nlogn), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    mymap& operator=(const mymap& other) {
        clear();

        _copy(other.root);

        return *this;
    }

    // clear:
    //
    // Frees the memory associated with the mymap; can be used for testing.
    // Time complexity: O(n), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    void clear() {
        _clear(root);

        root = nullptr;
    }

    //
    // destructor:
    //
    // Frees the memory associated with the mymap.
    // Time complexity: O(n), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    ~mymap() {
        _clear(root);

        root = nullptr;
    }

    //
    // put:
    //
    // Inserts the key/value into the threaded, self-balancing BST based on
    // the key.
    // Time complexity: O(logn + mlogm), where n is total number of nodes in the
    // threaded, self-balancing BST and m is the number of nodes in the
    // sub-tree that needs to be re-balanced.
    // Space complexity: O(1)
    //
    void put(keyType key, valueType value) {
        NODE* prev = nullptr;
        NODE* curr = root;
        NODE* broke = nullptr;
        NODE* parent = nullptr;
        NODE* brokeParent = nullptr;

        // Traverse the tree along the proper path until we reach the end
        while (curr != nullptr) {
            if (key == curr->key) {
                curr->value = value;
                return;
            }
            parent = prev;
            prev = curr;
            if (key < curr->key) {
                curr->nL++;
                curr = curr->left;
            } else if (curr->isThreaded) {
                curr->nR++;
                curr = nullptr;
            } else {
                curr->nR++;
                curr = curr->right;
            }

            // if the balancing condition is broken, then store the node where
            // it broke, plus its parent
            if (!isBalanced(prev)) {
                broke = prev;
                brokeParent = parent;
            }
        }

        // Create the node and put it where we fell out
        NODE* n = makeNode(key, value);

        // Match up all the pointers
        if (prev == nullptr) {
            root = n;
        } else if (key < prev->key) {
            prev->left = n;
            n->right = prev;
        } else {
            n->right = prev->right;
            prev->right = n;
            prev->isThreaded = false;
        }

        // if broken isn't a nullptr, we have to rebalance
        if (broke != nullptr) {
            if (brokeParent == nullptr) {
                root = rebalance(broke);
            } else if (broke == brokeParent->left) {
                brokeParent->left = rebalance(broke);
            } else {
                brokeParent->right = rebalance(broke);
            }
        }

        size++;
    }

    //
    // contains:
    // Returns true if the key is in mymap, return false if not.
    // Time complexity: O(logn), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    bool contains(keyType key) {
        NODE* curr = root;

        // Search tree till we find a match or fall out
        while (curr != nullptr) {
            if (key == curr->key) {
                return true;
            }

            if (key < curr->key) {
                curr = curr->left;
            } else if (curr->isThreaded) {
                curr = nullptr;
            } else {
                curr = curr->right;
            }
        }

        // If we reach here, we didn't find it in the tree
        return false;
    }

    //
    // get:
    //
    // Returns the value for the given key; if the key is not found, the
    // default value, valueType(), is returned (but not added to mymap).
    // Time complexity: O(logn), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    valueType get(keyType key) {
        NODE* curr = root;

        while (curr != nullptr) {
            if (key == curr->key) {
                return curr->value;
            }
            if (key < curr->key) {
                curr = curr->left;
            } else if (curr->isThreaded) {
                return valueType();
            } else {
                curr = curr->right;
            }
        }

        return valueType();
    }

    //
    // operator[]:
    //
    // Returns the value for the given key; if the key is not found,
    // the default value, valueType(), is returned (and the resulting new
    // key/value pair is inserted into the map).
    // Time complexity: O(logn + mlogm), where n is total number of nodes in the
    // threaded, self-balancing BST and m is the number of nodes in the
    // sub-trees that need to be re-balanced.
    // Space complexity: O(1)
    //
    valueType operator[](keyType key) {
        NODE* curr = root;

        while (curr != nullptr) {
            if (key == curr->key) {
                return curr->value;
            }
            if (key < curr->key) {
                curr = curr->left;
            } else if (curr->isThreaded) {
                put(key, valueType());
                return valueType();
            } else {
                curr = curr->right;
            }
        }

        // if we're here, key wasn't found so add it to the map
        put(key, valueType());

        // and return default value
        return valueType();
    }

    //
    // Size:
    //
    // Returns the # of key/value pairs in the mymap, 0 if empty.
    // O(1)
    //
    int Size() {
        return size;
    }

    //
    // begin:
    //
    // returns an iterator to the first in order NODE.
    // Time complexity: O(logn), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    iterator begin() {
        NODE* curr = root;

        while (curr->left != nullptr) {
            curr = curr->left;
        }
        return iterator(curr);
    }

    //
    // end:
    //
    // returns an iterator to the last in order NODE.
    // this function is given to you.
    // 
    // Time Complexity: O(1)
    //
    iterator end() {
        return iterator(nullptr);
    }

    //
    // toString:
    //
    // Returns a string of the entire mymap, in order.
    // Format for 8/80, 15/150, 20/200:
    // "key: 8 value: 80\nkey: 15 value: 150\nkey: 20 value: 200\n
    // Time complexity: O(n), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    string toString() {
        NODE* n = root;
        stringstream ss;

        _toString(n, ss);

        return ss.str();
    }

    //
    // toVector:
    //
    // Returns a vector of the entire map, in order.  For 8/80, 15/150, 20/200:
    // {{8, 80}, {15, 150}, {20, 200}}
    // Time complexity: O(n), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    vector<pair<keyType, valueType> > toVector() {
        NODE* n = root;
        vector<pair<keyType, valueType>> vec;

        _toVector(n, vec);

        return vec;
    }

    //
    // checkBalance:
    //
    // Returns a string of mymap that verifies that the tree is properly
    // balanced.  For example, if keys: 1, 2, 3 are inserted in that order,
    // function should return a string in this format (in pre-order):
    // "key: 2, nL: 1, nR: 1\nkey: 1, nL: 0, nR: 0\nkey: 3, nL: 0, nR: 0\n";
    // Time complexity: O(n), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    string checkBalance() {
        NODE* n = root;
        stringstream ss;

        _checkBalance(n, ss);

        return ss.str();
    }
};
