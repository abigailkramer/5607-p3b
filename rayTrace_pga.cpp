//CSCI 5607 HW3 - Rays & Files
//This HW has three steps:
// 1. Compile and run the program (the program takes a single command line argument)
// 2. Understand the code in this file (rayTrace_pga.cpp), in particular be sure to understand:
//     -How ray-sphere intersection works
//     -How the rays are being generated
//     -The pipeline from rays, to intersection, to pixel color
//    After you finish this step, and understand the math, take the HW quiz on canvas
// 3. Update the file parse_pga.h so that the function parseSceneFile() reads the passed in file
//     and sets the relevant global variables for the rest of the code to product to correct image

//To Compile: g++ -fsanitize=address -std=c++11 rayTrace_pga.cpp

//For Visual Studios
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

//Images Lib includes:
#define STB_IMAGE_IMPLEMENTATION //only place once in one .cpp file
#define STB_IMAGE_WRITE_IMPLEMENTATION //only place once in one .cpp files
#include "image_lib.h" //Defines an image class and a color class

//enables use of M_PI
#define _USE_MATH_DEFINES
#define INF ((unsigned) ~0)

//#3D PGA
#include "PGA_3D.h"

//High resolution timer
#include <chrono>

#include <omp.h>

//Scene file parser
#include "parse_pga.h"


Color ApplyLightingModel(Point3D rayStart, Line3D rayLine,HitInformation hitInfo, int depth);
Color EvaluateRayTree(Point3D rayStart, Line3D rayLine, int depth);

float clamp(float min, float max, float x)
{ return (x<min) ? min : (x>max) ? max : x; }

bool raySphereIntersect_fast(Point3D rayStart, Line3D rayLine, Point3D sphereCenter, float sphereRadius, HitInformation *hit){
  Dir3D dir = rayLine.dir();
  float a = dot(dir,dir);
  Dir3D toStart = (rayStart - sphereCenter);
  float b = 2 * dot(dir,toStart);
  float c = dot(toStart,toStart) - sphereRadius*sphereRadius;
  float discr = b*b - 4*a*c;
  if (discr < 0) return false;
  else{

    float t0 = (-b + sqrt(discr))/(2*a);
    float t1 = (-b - sqrt(discr))/(2*a);

    if (t0 > 0 || t1 > 0) return true;
  }
  return false;
}

bool rayTriangleIntersect(Point3D rayStart, Line3D rayLine, Triangle t, HitInformation *hit) {
  Plane3D triPlane = vee(t.v1,t.v2,t.v3).normalized();
  Point3D hitPoint = Point3D(wedge(rayLine,triPlane));
  Dir3D edge1 = t.v2 - t.v1;
  Dir3D edge2 = t.v3 - t.v1;

  double a = dot(edge1,cross(rayLine.dir(),edge2));
  if (a == 0) return false;

  double f = 1.0/a;
  double u = f*dot(rayStart - t.v1,cross(rayLine.dir(),edge2));
  if (u < 0.0 || u > 1.0) return false;

  double v = f*dot(rayLine.dir(),cross(rayStart - t.v1,edge1));
  if (v < 0.0 || u+v > 1.0) return false;
 
  Dir3D triNorm = cross(edge2,edge1).normalized();
  if (dot(triNorm,rayLine.dir()) > 0) {
    triNorm = triNorm*-1;
  }

  double w = f*dot(edge2,cross(rayStart - t.v1,edge1));
  if (w > 0.00000001) {
    hit->hit_point = hitPoint;
    if (t.is_normal) {
      float a = vee(t.v2,t.v3,hitPoint).magnitude() / triPlane.magnitude();   //v1
      float b = vee(t.v3,t.v1,hitPoint).magnitude() / triPlane.magnitude();   //v2
      float c = vee(t.v1,t.v2,hitPoint).magnitude() / triPlane.magnitude();   //v3
      hit->normal = (t.n1*a + t.n2*b + t.n3*c).normalized();
    }
    else hit->normal = vee(hit->hit_point,triNorm).normalized();
    return true;
  } 
  else return false;
}

bool raySphereIntersect(Point3D rayStart, Line3D rayLine, Point3D sphereCenter, float sphereRadius, HitInformation *hit) {
  Point3D projPoint = dot(rayLine,sphereCenter)*rayLine;      //Project to find closest point between circle center and line [proj(sphereCenter,rayLine);]
  float distSqr = projPoint.distToSqr(sphereCenter);          //Point-line distance (squared)
  float d2 = distSqr/(sphereRadius*sphereRadius);             //If distance is larger than radius, then...
  if (d2 > 1) return false;                                   //... the ray missed the sphere
  float w = sphereRadius*sqrt(1-d2);                          //Pythagorean theorem to determine dist between proj point and intersection points
  Point3D p1 = projPoint - rayLine.dir()*w;                   //Add/subtract above distance to find hit points
  Point3D p2 = projPoint + rayLine.dir()*w; 

  if (dot((p1-rayStart),rayLine.dir()) >= 0){
    hit->hit_point = p1;
    return true;     //Is the first point in same direction as the ray line?
  }
  if (dot((p2-rayStart),rayLine.dir()) >= 0){    
    hit->hit_point = p2;
    return true;     //Is the second point in same direction as the ray line?
  }
  return false;
}

// returns true/false - &info = the closest intersection HitInformation
// goes through all primitives
bool intersect(Point3D rayStart, Line3D rayLine, HitInformation *info) {
  double currentDist = INF;                              // start as far away as possible
  HitInformation test = HitInformation();
  bool hit = false;

  for (auto& s : spheres) {
    if (raySphereIntersect(rayStart,rayLine,s.pos,s.radius,&test)) {
      if (rayStart.distTo(test.hit_point) > currentDist) continue;     // move from back to front
      currentDist = rayStart.distTo(test.hit_point);

      hit = true;
      info->hit_point = test.hit_point;
      info->t = rayStart.distTo(info->hit_point);
      info->normal = vee(s.pos,info->hit_point).normalized();
      info->ambient = s.ambient;
      info->diffuse = s.diffuse;
      info->specular = s.specular;
      info->transmissive = s.transmissive;
      info->ns = s.ns;
      info->ior = s.ior;
    }
  }

  for (auto& t : triangles) {
    if (rayTriangleIntersect(rayStart,rayLine,t,&test)) {
      if (rayStart.distTo(test.hit_point) > currentDist) continue;
      currentDist = rayStart.distTo(test.hit_point);

      hit = true;
      info->hit_point = test.hit_point;
      info->t = rayStart.distTo(info->hit_point);
      info->normal = test.normal;
      info->ambient = t.ambient;
      info->diffuse = t.diffuse;
      info->specular = t.specular;
      info->transmissive = t.transmissive;
      info->ns = t.ns;
      info->ior = t.ior;
    }
  }

  return hit;
}

Line3D Reflect (Line3D ray, Line3D normal, Point3D hit){
  Plane3D plane = dot(normal, hit);
  return sandwhich(plane,ray);
}

Line3D Refract(Line3D ray, Line3D normal, float ior, float nr) {  // nr should be 1.0 initially
  float cos_i = -clamp(-1,1,dot(ray,normal));
  if (cos_i < 0) return Refract(ray, (normal*-1), nr, ior);

  float n = nr / ior;
  float cos_r = sqrtf(1.f - n*n*(1.f - cos_i*cos_i));
  return ray*n + normal*(n*cos_i - cos_r);
}

Color ApplyLightingModel(Point3D rayStart, Line3D rayLine, HitInformation hitInfo, int depth) {
  Color color = Color(0,0,0);
  Point3D p = hitInfo.hit_point;
  Point3D hit_point = p + hitInfo.normal.dir()*0.001;
  Dir3D N = hitInfo.normal.dir();
  Dir3D V = vee(p,eye).dir().normalized();

  for (auto& light : dir_lights) {
    Dir3D L = light.direction.normalized()*-1;
    Line3D shadow = vee(p,L).normalized();
    HitInformation shadow_hit = HitInformation();
    bool blocked = false;

    if (intersect(hit_point,shadow,&shadow_hit)) blocked = true;
    if (blocked) continue;

    float n_l = std::max(dot(N,L),0.f);
    Dir3D R = (L - 2*(dot(L,N)*N)).normalized()*-1;
    float v_r = pow(std::max(dot(V,R),0.f), hitInfo.ns);

    Color contribution = ((hitInfo.diffuse*n_l) + (hitInfo.specular*v_r)) * light.intensity;
    color = color + contribution;
  } // directional lights

  for (auto& light : point_lights) {
    HitInformation shadow_hit = HitInformation();
    bool blocked = false;
    Dir3D L = (light.location - p).normalized();
    Line3D lightLine = vee(light.location,L).normalized();
    Line3D shadow = vee(hit_point,light.location).normalized();

    double dist = hit_point.distTo(light.location); 

    if (intersect(hit_point,shadow,&shadow_hit)) blocked = true; 
    if (blocked && shadow_hit.t < dist) continue;

    float n_l = std::max(dot(N,L),0.f);
    Dir3D R = Reflect(lightLine,hitInfo.normal,hit_point).dir();
    float v_r = pow(std::max(dot(V,R),0.f), hitInfo.ns);

    Color contribution = ((hitInfo.diffuse*n_l) + (hitInfo.specular*v_r));
    float attenuation = 1.0 / (1.0 + dist*dist);
    contribution = contribution*(light.intensity*attenuation);
    color = color + contribution;
  } // point lights

  for (auto& light : spot_lights) {
    HitInformation shadow_hit = HitInformation();
    bool blocked = false;
    Dir3D L = (light.location - p).normalized();
    Line3D shadow = vee(hit_point, light.location).normalized();
    Line3D lightLine = vee(light.location,L);

    if (intersect(hit_point,shadow,&shadow_hit)) blocked = true;

    float angle = acos(dot(shadow.normalized(),lightLine.normalized()));
    angle *= (180.0f/M_PI);
    double dist = p.distTo(light.location);  
    if ((blocked && shadow_hit.t < dist) || (angle > light.angle2)) continue;

    float attenuation = 1.0 / (1.0 + dist*dist);
    if (angle < light.angle1) attenuation = 1.0 / dist;    

    float n_l = std::max(dot(N,L),0.f);
    Dir3D R = Reflect(lightLine,hitInfo.normal,hit_point).dir();
    float v_r = pow(std::max(dot(V,R),0.f), hitInfo.ns);

    Color contribution = ((hitInfo.diffuse*n_l) + (hitInfo.specular*v_r));
    contribution = contribution*(light.intensity*attenuation);
    color = color + contribution;

  } // spot lights

  Line3D mirror = Reflect(rayLine,hitInfo.normal,hit_point).normalized();
  color = color + (hitInfo.specular * EvaluateRayTree(hit_point,mirror,depth+1));

  bool outside = -dot(rayLine,hitInfo.normal) < 0;
  Point3D refract_pt = outside ? p + hitInfo.normal.dir()*0.001 : p - hitInfo.normal.dir()*0.001;
  Line3D glass = Refract(rayLine,hitInfo.normal,hitInfo.ior,1.0).normalized();
  color = color + (hitInfo.transmissive * EvaluateRayTree(refract_pt,glass,depth+1));

  color = color + (ambient_light*hitInfo.ambient);
  return color;
} // ApplyLightingModel

Color EvaluateRayTree(Point3D rayStart, Line3D rayLine, int depth) {
  bool hit_something;
  HitInformation hit = HitInformation();

  hit_something = intersect(rayStart,rayLine,&hit);
  if (hit_something && depth < max_depth) { 
    return ApplyLightingModel(rayStart,rayLine,hit,depth); 
  } else {
    return background;
  }
}

int main(int argc, char** argv){  
  //Read command line paramaters to get scene file
  if (argc != 2){
     std::cout << "Usage: ./a.out scenefile\n";
     return(0);
  }
  std::string secenFileName = argv[1];

  //Parse Scene File
  parseSceneFile(secenFileName);

  float imgW = img_width, imgH = img_height;
  float halfW = imgW/2, halfH = imgH/2;
  float d = halfH / tanf(halfAngleVFOV * (M_PI / 180.0f));

  Image outputImg = Image(img_width,img_height);
  auto t_start = std::chrono::high_resolution_clock::now();

  // basic sampling
  #pragma omp parallel for schedule(dynamic, 5)
  for (int i = 0; i < img_width; i++) {
    for (int j = 0; j < img_height; j++) {
      float u = (halfW - (imgW)*((i+0.5)/imgW));
      float v = (halfH - (imgH)*((j+0.5)/imgH));
      Point3D p = eye - d*forward + u*right + v*up;
      Dir3D rayDir = (p - eye); 
      Line3D rayLine = vee(eye,rayDir).normalized();

      Color color = EvaluateRayTree(eye,rayLine,0);
      outputImg.setPixel(i,j,color);
    }
  }

  auto t_end = std::chrono::high_resolution_clock::now();
  printf("Rendering took %.2f ms\n",std::chrono::duration<double, std::milli>(t_end-t_start).count());

  outputImg.write(imgName.c_str());
  return 0;
}
