#include <iostream>
#include "vec3.h"
#include "stb_image_write.h"

void write_to_png(int nx, int ny, int comp, void *data) {
  int stride_in_bytes = comp * nx;
  stbi_write_png("output.png", nx, ny, comp, data, stride_in_bytes);
}

void output_ppm(int nx, int ny, void *data) {
  unsigned char *p = (unsigned char*)data;

  std::cout << "P3\n" << nx << " " << ny << "\n255\n";
  for (int y = 0; y < ny; y++) {
    for (int x = 0; x < nx; x++) {
      std::cout << (unsigned int)p[0] << " " << (unsigned int)p[1] << " " << (unsigned int)p[2] << "\n";
      p += 3;
    }
  }
}

int main() {
  int nx = 200;
  int ny = 100;
  int comp = 3; //RGB
  unsigned char *data = new unsigned char[nx * ny * comp];
  unsigned char *p = data;

  for (int j = ny - 1; j >= 0; j--) {
    for (int i = 0 ;i < nx; i++ ) {
      vec3 col(float(i) / float(nx), float(j) / float(ny), 0.2);
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