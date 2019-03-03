#include <iostream>
#include "ray.h"
#include "stb_image_write.h"

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

vec3 color_ch01_ch02(float u, float v) {
  return vec3(u, v, 0.2);
}

vec3 color(const ray& r) {
  vec3 unit_direction = unit_vector(r.direction());
  float t = 0.5 * (unit_direction.y() + 1.0); // -1.0 ~ 1.0 -> 0.0 ~ 1.0
  return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3 (0.5, 0.7, 1.0); //lerp between white and blue
}

int main() {
  int nx = 200;
  int ny = 100;
  int comp = 3; //RGB
  unsigned char* data = new unsigned char[nx * ny * comp];
  unsigned char* p = data;

  vec3 lower_left_corner(-2.0, -1.0, -1.0);
  vec3 horizontal(4.0, 0.0, 0.0);
  vec3 vertical(0.0, 2.0, 0.0);
  vec3 origin(0.0, 0.0, 0.0);
  for (int j = ny - 1; j >= 0; j--) {
    for (int i = 0 ;i < nx; i++ ) {
      float u = float(i) / float(nx);
      float v = float(j) / float(ny);
      ray r(origin, lower_left_corner + u * horizontal + v * vertical);
      vec3 col = color(r);
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