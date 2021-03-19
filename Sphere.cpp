#include "Sphere.h"
#include "PGA_3D.h"

Sphere::Sphere(Material material, Point3D position, float rad) {
    objMaterial = material;
    pos = position;
    radius = rad;
}

Sphere::~Sphere(void) { }

bool Sphere::Intersect(Point3D rayStart, Line3D rayLine, HitInformation& hit) {
    
    // bool intersect = false;

    // Dir3D d = radius - rayStart;
    // float a = dot(rayLine.dir(),d);

    // float f = radius*radius - dot(d,d) + (a*a);

    // if (f < 0.f) return intersect;
    // else {
    //     float t = a - sqrtf(f);
    //     hit.t = t;
    //     hit.hit_point = (rayStart + d*t);
    //     result = true;
    // }
    
    Point3D projPoint = dot(rayLine,pos)*rayLine;
    float distSqr = projPoint.distToSqr(pos);
    float d2 = distSqr/(radius*radius);
    if (d2 > 1) return false;
    float w = radius*sqrt(1-d2);
    Point3D p1 = projPoint - rayLine.dir()*w;
    Point3D p2 = projPoint + rayLine.dir()*w; 

    if (dot((p1-rayStart),rayLine.dir()) >= 0){
        hit.hit_point = p1;
        return true;
    }
    if (dot((p2-rayStart),rayLine.dir()) >= 0){    
        hit.hit_point = p2;
        return true;
    }
    return false;
}