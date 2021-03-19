#include "PGA_3D.h"

class Object {
public:
    Object(Material material);
    ~Object();

    Material*       GetMaterial()
    virtual Line3D  GetNormal(Line3D& position)=0;

    virtual bool    Intersect(Point3D rayStart, Line3D rayLine, HitInformation& hitInfo)=0;

protected:
    Material objMaterial;
};