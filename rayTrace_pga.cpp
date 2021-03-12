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

//Scene file parser
#include "parse_pga.h"


Color ApplyLightingModel(Point3D rayStart, Line3D rayLine,HitInformation hitInfo, int depth);
Color EvaluateRayTree(Point3D rayStart, Line3D rayLine, int depth);

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

  return false;
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
    hit->t = w;
    hit->hit_point = p1;
    hit->normal = vee(sphereCenter,hit->hit_point).normalized();
    return true;     //Is the first point in same direction as the ray line?
  }
  if (dot((p2-rayStart),rayLine.dir()) >= 0){    
    hit->t = w;
    hit->hit_point = p2;
    hit->normal = vee(sphereCenter,hit->hit_point).normalized();
    return true;     //Is the second point in same direction as the ray line?
  }
  return false;
}

bool intersect(Point3D rayStart, Line3D rayLine, HitInformation *info) {
  double currentDist = INF;                              // start as far away as possible
  HitInformation possible = HitInformation();
  bool hit = false;

  for (auto& s : spheres) {
    if (raySphereIntersect(rayStart,rayLine,s.pos,s.radius,&possible)) {
      if (rayStart.distTo(possible.hit_point) > currentDist) continue;     // move from back to front
      currentDist = rayStart.distTo(possible.hit_point);

      hit = true;
      info->hit_point = possible.hit_point;
      info->t = possible.t;
      info->normal = possible.normal;
      info->ambient = s.ambient;
      info->diffuse = s.diffuse;
      info->specular = s.specular;
      info->transmissive = s.transmissive;
      info->ns = s.ns;
      info->ior = s.ior;
    }
  }
  return hit;
}

Line3D Reflect (Line3D ray, Line3D normal, Point3D hit){
  Plane3D plane = dot(normal, hit);
  // ray = -1*MultiVector(ray);
  return sandwhich(plane,ray);
}

Color ApplyLightingModel(Point3D rayStart, Line3D rayLine, HitInformation hitInfo, int depth) {
  Color color = Color(0,0,0);
  Point3D p = hitInfo.hit_point;
  Point3D shadow_point = p + hitInfo.normal.dir()*0.001;
  Dir3D N = hitInfo.normal.dir();
  Dir3D V = vee(p,eye).dir().normalized();

  for (auto& light : dir_lights) {
    Dir3D L = light.direction.normalized();
    Line3D shadow = vee(p,L).normalized();
    HitInformation shadow_hit = HitInformation();
    bool blocked = false;

    for (auto& s : spheres) {
      if (raySphereIntersect(shadow_point,shadow,s.pos,s.radius,&shadow_hit)) blocked = true;
    }
    if (blocked) continue;

    float n_l = std::max(dot(N,L),0.f);
    Dir3D R = (L - 2*(dot(L,N)*N)).normalized();
    float v_r = pow(std::max(dot(V,R),0.f), hitInfo.ns);

    Color contribution = ((hitInfo.diffuse*n_l) + (hitInfo.specular*v_r)) * light.intensity;
    color = color + contribution;
  } // directional lights

  for (auto& light : point_lights) {
    HitInformation shadow_hit = HitInformation();
    bool blocked = false;
    Dir3D L = (light.location - p).normalized();
    Line3D lightLine = vee(light.location,L).normalized();
    Line3D shadow = vee(shadow_point,light.location).normalized();

    for (auto& s : spheres) {
      if (raySphereIntersect(shadow_point,shadow,s.pos,s.radius,&shadow_hit)) blocked = true;
    }

    double dist = p.distTo(light.location);  
    if (blocked && shadow_hit.t < dist) continue;

    float n_l = std::max(dot(N,L),0.f);
    Line3D line = -1*MultiVector(lightLine);
    Dir3D R = Reflect(line,hitInfo.normal,shadow_point).dir();
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
    Line3D shadow = vee(shadow_point, light.location).normalized();
    Line3D lightLine = vee(light.location,L);

    for (auto& s : spheres) {
      if (raySphereIntersect(shadow_point,shadow,s.pos,s.radius,&shadow_hit)) blocked = true;
    }

    float angle = acos(dot(shadow.normalized(),lightLine.normalized()));
    angle *= (180/M_PI);
    double dist = p.distTo(light.location);  
    if (blocked && shadow_hit.t < dist || (angle > light.angle2)) continue;

    float attenuation = 1.0 / (1.0 + dist*dist);
    if (angle < light.angle1) attenuation = 1.0 / dist;    

    float n_l = std::max(dot(N,L),0.f);
    Dir3D R = Reflect(lightLine,hitInfo.normal,shadow_point).dir();
    float v_r = pow(std::max(dot(V,R),0.f), hitInfo.ns);

    Color contribution = ((hitInfo.diffuse*n_l) + (hitInfo.specular*v_r));
    contribution = contribution*(light.intensity*attenuation);
    color = color + contribution;

  } // spot lights

  Line3D mirror = Reflect(rayLine,hitInfo.normal,shadow_point).normalized();
  Color mirror_color = EvaluateRayTree(shadow_point,mirror,depth+1);
  color = color + (hitInfo.specular * mirror_color);

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

  for (int i = 0; i < img_width; i++) {
    for (int j = 0; j < img_height; j++) {
      float u = (halfW - (imgW)*((i+0.5)/imgW));
      float v = (halfH - (imgH)*((j+0.5)/imgH));
      Point3D p = eye - d*forward + u*right + v*up;
      Dir3D rayDir = (p - eye); 
      Line3D rayLine = vee(eye,rayDir).normalized();  //Normalizing here is optional

      Color color = EvaluateRayTree(eye,rayLine,0);
      outputImg.setPixel(i,j,color);

    }
  }

  auto t_end = std::chrono::high_resolution_clock::now();
  printf("Rendering took %.2f ms\n",std::chrono::duration<double, std::milli>(t_end-t_start).count());

  outputImg.write(imgName.c_str());
  return 0;
}
