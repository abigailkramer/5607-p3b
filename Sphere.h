#pragma once

#include "Object.h"
#include "PGA_3D.h"

class Sphere : public Object {
public:
    Sphere(Material material, Point3D position, float rad);
    ~Sphere();

    Point3D&    GetRadius()     { return pos; }
    Line3D      GetNormal(Point3D hit)     { return  vee(pos,hit).normalized(); }

    bool Intersect(Point3D rayStart, Line3D rayLine, HitInformation& hitInfo);

private:
    Point3D pos;
    float radius;
};