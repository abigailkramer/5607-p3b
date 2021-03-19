#include "PGA_3D.h"
#include <vector>
#include "Object.h"

class Node {

public:

    virtual bool isLeaf()=0;
    
    std::vector<Object*> objects;   //bc abstract
    int node_min;
    int node_max;
    Node* left_child;
    Node* right_child;

    // node functions
    void addObject(Object *obj);
    ~Node();
};

class InnerNode : public Node {

public:

    InnerNode(int min, int max, Node* left, Node* right) { node_min = min; node_max = max; left_child = left; right_child = right;};

    bool isLeaf() { return false; }

    // std::vector<Object> objects;
};

class LeafNode : public Node {

    LeafNode(int min, int max) { node_min = min; node_max = max; left_child = NULL; right_child = NULL;}
    LeafNode(LeafNode& s) { *this = s; }

    bool isLeaf() { return true; }
};