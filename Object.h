#include "PGA_3D.h"

class Object {
public:
    // Object(Material material);
    // ~Object();

    virtual Line3D  GetNormal(Line3D& position)=0;
    // Line3D          GetNormal(Line3D& position);

    // void SetMaterial(Material m) {
    //     objMaterial = m;
    // }

    virtual bool    Intersect(Point3D rayStart, Line3D rayLine, HitInformation& hitInfo)=0;
    Material objMaterial;
};