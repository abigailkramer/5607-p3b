#include "PGA_3D.h"
#include "Object.h"
#include "Node.h"

class BVH {

public:

    BVH();
    ~BVH() {if (root) {root->~Node();} }

    Node* GetRoot() { return root; }

protected:

    Node* root;
    int max_nodes;

};