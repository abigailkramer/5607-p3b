
//Set the global scene parameter variables
//TODO: Set the scene parameters based on the values in the scene file

#ifndef PARSE_PGA_H
#define PARSE_PGA_H

#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

//Camera & Scene Parmaters (Global Variables)
//Here we set default values, override them in parseSceneFile()

//Image Parmaters
int img_width = 640, img_height = 480;
std::string imgName = "raytraced.bmp";

//Camera Parmaters
Point3D eye = Point3D(0,0,0); 
Dir3D forward = Dir3D(0,0,-1).normalized();
Dir3D up = Dir3D(0,1,0).normalized();
Dir3D right = Dir3D(1,0,0).normalized();
float halfAngleVFOV = 45; 

//Scene Parameters
int max_vertices = 0;
int max_normals = 0;
std::vector<Point3D> vertices;    // allows access by number
int vert_num = 0;
std::vector<Dir3D> normals;
int norm_num = 0;

// add triangle (v1,v2,v2) & normal_triangle (v1,v2,v3,n1,n2,n3)

Color background = Color(0,0,0);

std::vector<Sphere> spheres;
std::vector<Triangle> triangles;
std::vector<NormalTriangle> norm_triangles;

//Material Parameters - to set as "state" vars
Color ambient_color = Color(0,0,0);
Color diffuse_color = Color(1,1,1);
Color specular_color = Color(0,0,0);
int ns = 5;
Color transmissive_color = Color(0,0,0);
float ior = 1;

//Lighting Parameters  -- make structs for each light type? to be able to go through them?
std::vector<DirLight> dir_lights;
std::vector<PointLight> point_lights;
std::vector<SpotLight> spot_lights;

Color ambient_light = Color(0,0,0);

//Misc.
int max_depth = 5;


void parseSceneFile(std::string fileName){
  //TODO: Override the default values with new data from the file "fileName"

  std::ifstream file(fileName);

  if (file.fail()) {
    std::cout << "Can't open file '" << fileName << "'" << std::endl;
  }

  std::string line;
  std::string cmd;
  while (file >> cmd) {     // read first word in line -- the cmd

    if (cmd[0] == '#') {
      std::getline(file, line);   // skip the rest of the line
      std::cout << "Skipping comment: " << cmd  << line <<  std::endl;
      continue;
    }


    if (cmd == "camera_pos:") {
      float x,y,z;
      file >> x >> y >> z;
      eye = Point3D(x,y,z);
    } 
    else if (cmd == "camera_fwd:") {
      float fx,fy,fz;
      file >> fx >> fy >> fz;
      forward = Dir3D(fx,fy,fz).normalized();
    } 
    else if (cmd == "camera_up:") {
      float ux,uy,uz;
      file >> ux >> uy >> uz;
      up = Dir3D(ux,uy,uz).normalized();
    } 
    else if (cmd == "camera_fov_ha:") {
      float angle;
      file >> angle;
      halfAngleVFOV = angle;
    }
    else if (cmd == "film_resolution:") {
      int width,height;
      file >> width >> height;
      img_width = width;
      img_height = height;
    } 
    else if (cmd == "output_image:") {
      std::string outFile;
      file >> outFile;
      imgName = outFile;
    }
// Scene Geometry
    else if (cmd == "max_vertices:") {
      int max;
      file >> max;
      max_vertices = max;
    }
    else if (cmd == "max_normals:") {
      int max;
      file >> max;
      max_normals = max;
    }
    else if (cmd == "vertex:") {
      float x,y,z;
      file >> x >> y >> z;
      if (vert_num == max_vertices) {
        continue;
      }
      vertices.push_back(Point3D(x,y,z));
      vert_num++;
    }
    else if (cmd == "normal:") {
      float x,y,z;
      file >> x >> y >> z;
      if (norm_num == max_normals) {
        continue;
      }
      normals.push_back(Dir3D(x,y,z));
      norm_num++;
    }
    else if (cmd == "triangle:") {
      int v1,v2,v3;
      file >> v1 >> v2 >> v3;
      Triangle t = Triangle();
      t.v1 = vertices[v1];
      t.v2 = vertices[v2];
      t.v3 = vertices[v3];
      triangles.push_back(t);
    }
    else if (cmd == "normal_triangle:") {
      int v1,v2,v3,n1,n2,n3;
      file >> v1 >> v2 >> v3 >> n1 >> n2 >> n3;
      NormalTriangle t = NormalTriangle();
      t.v1 = vertices[v1];
      t.v2 = vertices[v2];
      t.v3 = vertices[v3];
      t.n1 = normals[v1];
      t.n2 = normals[v2];
      t.n3 = normals[v3];     
      norm_triangles.push_back(t);       
    }
    else if (cmd == "sphere:") {
      float x,y,z,r;
      file >> x >> y >> z >> r;
      Sphere s = Sphere();
      s.pos = Point3D(x,y,z);
      s.radius = r;

      // set material variables based on current state
      s.ambient = ambient_color;
      s.diffuse = diffuse_color;
      s.specular = specular_color;
      s.transmissive = transmissive_color;
      s.ns = ns;
      s.ior = ior;

      spheres.push_back(s);
    }
    else if (cmd == "background:") {
      float r,g,b;
      file >> r >> g >> b;
      background = Color(r,g,b);
    }
// Material Parameters
    else if (cmd == "material:") {
      float ar,ag,ab,dr,dg,db,sr,sg,sb,tr,tg,tb,io;
      int n;
      file >> ar >> ag >> ab >> dr >> dg >> db;
      file >> sr >> sg >> sb >> n >> tr >> tg >> tb >> io;

      // set new Material state variables
      ambient_color = Color(ar,ag,ab);
      diffuse_color = Color(dr,dg,db);
      specular_color = Color(sr,sg,sb);
      transmissive_color = Color(tr,tg,tb);
      ns = n;
      ior = io;    
    }
// Lighting Parameters
    else if (cmd == "directional_light:") {
      float r,g,b,x,y,z;
      file >> r >> g >> b >> x >> y >> z;
      DirLight d = DirLight();
      d.direction = Dir3D(x,y,z);
      d.intensity = Color(r,g,b);
      dir_lights.push_back(d);
    }
    else if (cmd == "point_light:") {
      float r,g,b,x,y,z;
      file >> r >> g >> b >> x >> y >> z;
      PointLight p = PointLight();
      p.intensity = Color(r,g,b);
      p.location = Point3D(x,y,z);
      point_lights.push_back(p);
    }
    else if (cmd == "spot_light:") {
      float r,g,b,px,py,pz,dx,dy,dz,a1,a2;
      file >> r >> g >> b >> px >> py >> pz;
      file >> dx >> dy >> dz >> a1 >> a2;
      SpotLight s = SpotLight();
      s.intensity = Color(r,g,b);
      s.location = Point3D(px,py,pz);
      s.direction = Dir3D(dx,dy,dz);
      s.angle1 = a1;
      s.angle2 = a2;
      spot_lights.push_back(s);
    }
    else if (cmd == "ambient_light:") {
      float r,g,b;
      file >> r >> g >> b;
      ambient_light = Color(r,g,b);
    }
// Miscellaneous
    else if (cmd == "max_depth:") {
      int n;
      file >> n;
      max_depth = n;
    }
    else {
      std::getline(file,line);
      std::cout << "WARNING. Unknow command: " << cmd  << line <<  std::endl;
    }

  }


  right = cross(up,forward);

  right.normalized();
  up.normalized();
  forward.normalized();

  // the vectors should already be normalized

  if ((dot(right,forward) != 0) || (dot(forward,up) != 0) || (dot(up,right) != 0)) {

    Dir3D new_up = (dot(up,forward) / dot(forward,forward))*forward;
    up = (up - new_up).normalized();

    Dir3D new_right = (dot(right,up) / dot(up,up))*up;
    right = (right - new_right).normalized();

    Dir3D new_forward = (dot(forward,right) / dot(right,right))*right;
    forward = (forward - new_forward).normalized();

  }

  //TODO: Create an orthagonal camera basis, based on the provided up and right vectors
  printf("Orthagonal Camera Basis:\n");
  forward.print("forward");
  right.print("right");
  up.print("up");

  file.close();
  return;
}

#endif