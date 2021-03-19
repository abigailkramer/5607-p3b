#include "PGA_3D.h"
#include <vector>
// #include "Object.h"

class BVHNode {

public:

    // BVHNode(Point3D node_min, Point3D node_max);
    BVHNode(int minx, int maxx, int miny, int maxy);
    BVHNode(int minx, int maxx, int miny, int maxy, int midx, int midy);
    ~BVHNode();
    
    // std::vector<Object*> objects;   //bc abstract
    std::vector<Sphere> spheres;
    std::vector<Triangle> triangles;
    double min_x, min_y;
    double max_x, max_y;
    double mid_x, mid_y;
    BVHNode* left_child;
    BVHNode* right_child;
    bool isLeaf;

    // node functions
    // void addObject(Object *obj);
    void addSphere(Sphere s);
    void addTriangle(Triangle t);
    BVHNode* getLeftChild() { return left_child; }
    BVHNode* getRightChild() { return right_child; }
    void addLeftChild(BVHNode* left);
    void addRightChild(BVHNode* right);
};