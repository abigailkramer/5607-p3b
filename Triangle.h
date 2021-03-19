#pragma once

#include "Object.h"
#include "PGA_3D.h"

class Triangle : public Object {
public:
    Triangle(Material material, Point3D pos, float radius);
    ~Triangle();

    Point3D&    GetRadius()     { return pos; }
    Line3D      GetNormal(Point3D hit)     { return  vee(pos,hit).normalized(); }

    bool Intersect(Point3D rayStart, Line3D rayLine, HitInformation& hitInfo);

private:
    Point3D pos;
    float radius;
};