#include "Node.h"
#include <vector>

void Node::addObject(Object *obj) {
    objects.push_back(obj);
}

Node::~Node() {
    if (right_child != NULL) delete right_child;
    if (left_child != NULL) delete left_child;
    delete this;
}