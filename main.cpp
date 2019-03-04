#include <iostream>
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "stb_image_write.h"
#include "float.h"
#include "stdlib.h"

double drand48() {
  return (double)rand() / RAND_MAX;
}

void write_to_png(int nx, int ny, int comp, void* data) {
  int stride_in_bytes = comp * nx;
  stbi_write_png("output.png", nx, ny, comp, data, stride_in_bytes);
}

void output_ppm(int nx, int ny, void* data) {
  unsigned char *p = (unsigned char*)data;

  std::cout << "P3\n" << nx << " " << ny << "\n255\n";
  for (int y = 0; y < ny; y++) {
    for (int x = 0; x < nx; x++) {
      std::cout << (unsigned int)p[0] << " " << (unsigned int)p[1] << " " << (unsigned int)p[2] << "\n";
      p += 3;
    }
  }
}

vec3 color(const ray& r, hitable* world) {
  hit_record rec;
  if (world->hit(r, 0.0, FLT_MAX, rec)) {
    return 0.5 * vec3(rec.normal.x()+1.0, rec.normal.y()+1.0, rec.normal.z()+1.0); //clamp to 0,1
  }
  else {
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0); // -1.0 ~ 1.0 -> 0.0 ~ 1.0
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3 (0.5, 0.7, 1.0); //lerp between white and blue
  }
}

int main() {
  int nx = 200;
  int ny = 100;
  int numsamples = 100;
  int comp = 3; //RGB
  unsigned char* data = new unsigned char[nx * ny * comp];
  unsigned char* p = data;

  hitable* list[2];
  list[0] = new sphere(vec3(0,0,-1), 0.5);
  list[1] = new sphere(vec3(0,-100.5,-1), 100);
  hitable* world = new hitable_list(list, 2);
  camera cam;
  for (int j = ny - 1; j >= 0; j--) {
    for (int i = 0 ;i < nx; i++ ) {
      vec3 col(0,0,0);
      for (int s = 0; s < numsamples; s++) {
        float u = float(i + drand48()) / float(nx);
        float v = float(j + drand48()) / float(ny);
        ray r = cam.get_ray(u,v);
        col += color(r, world);
      }
      col /= float(numsamples);
      int ir = int(255.99 * col[0]);
      int ig = int(255.99 * col[1]);
      int ib = int(255.99 * col[2]);
      *p++ = (unsigned char)ir;
      *p++ = (unsigned char)ig;
      *p++ = (unsigned char)ib;
    }
  }

  output_ppm(nx, ny, data);
  write_to_png(nx, ny, comp, data);

  delete[] data;
}