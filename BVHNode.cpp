#include "BVHNode.h"

BVHNode::BVHNode(int minx, int maxx, int miny, int maxy) {
    min_x = minx;
    max_x = maxx;
    min_y = miny;
    max_y = maxy;
}

BVHNode::BVHNode(int minx, int maxx, int miny, int maxy, int midx, int midy) {
    min_x = minx;
    max_x = maxx;
    min_y = miny;
    max_y = maxy;
    mid_x = midx;
    mid_y = midy;    
}

BVHNode::~BVHNode() { }

void BVHNode::addSphere(Sphere s){
    spheres.push_back(s);
}

void BVHNode::addTriangle(Triangle t) {
    triangles.push_back(t);
}

void BVHNode::addLeftChild(BVHNode* left) {
    left_child = left;
}

void BVHNode::addRightChild(BVHNode* right) {
    right_child = right;
}