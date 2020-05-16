#ifdef _MSC_VER 
#ifdef _CRT_SECURE_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#endif
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include <iostream>
#include "sphere.h"
#include "triangle.h"
#include "hitable_list.h"
#include "camera.h"
#include "stb_image_write.h"
#include <cfloat> //for FLT_MAX
#include <cstdlib>
#include <thread>
#include <random>


static std::uniform_real_distribution<double> distribution(0.0, 1.0); // [0.0, 1.0)
double drand48() {
    static thread_local std::mt19937 generator;
    return distribution(generator);
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

vec3 random_in_unit_sphere() {
  vec3 p;
  do {
    p = 2.0 * vec3(drand48(),drand48(),drand48()) - vec3(1,1,1);
  } while (p.squared_length() >= 1.0);
  return p;
}

vec3 color(const ray& r, hitable* world) {
  float EPSILON = 0.001; //to ignore hits very near zero
  hit_record rec;
  if (world->hit(r, EPSILON, FLT_MAX, rec)) {
//    return 0.5 * vec3(rec.normal.x()+1, rec.normal.y()+1, rec.normal.z()+1); //visualize surface normal
    vec3 target = rec.p + rec.normal + random_in_unit_sphere();
    return 0.5 * color(ray(rec.p, target - rec.p), world);
  }
  else {
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0); // -1.0 ~ 1.0 -> 0.0 ~ 1.0
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3 (0.5, 0.7, 1.0); //lerp between white and blue
  }
}

static int num_threads = 1;

void raytrace_thread(int thread_id, int nx, int ny, int comp, int numsamples, camera* cam, hitable* world, unsigned char* p) {
  for (int j = ny - 1; j >= 0; j--) {
    if (j % num_threads != thread_id) {
      p += nx * comp; // go to next row
      continue;
    }
    for (int i = 0 ;i < nx; i++ ) {
      vec3 col(0,0,0);
      for (int s = 0; s < numsamples; s++) {
        float u = float(i + drand48()) / float(nx);
        float v = float(j + drand48()) / float(ny);
        ray r = cam->get_ray(u,v);
        col += color(r, world);
      }
      col /= float(numsamples);
      col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2])); //gamma correct
      int ir = int(255.99 * col[0]);
      int ig = int(255.99 * col[1]);
      int ib = int(255.99 * col[2]);
      *p++ = (unsigned char)ir;
      *p++ = (unsigned char)ig;
      *p++ = (unsigned char)ib;
    }
  }
}


int main( int argc, char *argv[]) {
  const char* numcpus_env = getenv("NUMBER_OF_PROCESSORS"); //Windows
  if (numcpus_env) {
    num_threads = atoi(numcpus_env);
  }
  std::cout << "number of cpus detected:" << num_threads << '\n';


  int nx = 400;
  int ny = 200;
  int numsamples = 100;
  int comp = 3; //RGB
  unsigned char* data = new unsigned char[nx * ny * comp];
  unsigned char* p = data;

  int num_objects = 2;
  hitable** list = new hitable*[num_objects];
  list[0] = new sphere(vec3(0,0,-1), 0.5);
  list[1] = new sphere(vec3(0,-100.5,-1), 100);
//  list[2] = new triangle(vec3(0,-0.5,0), vec3(0,-0.5,-2), vec3(1,-0.5,0), vec3(0,1,0));
  hitable* world = new hitable_list(list, num_objects);
  camera cam;

  std::thread *threads = new std::thread[num_threads];
  for (int i = 0; i < num_threads; ++i) {
    threads[i] = std::thread(raytrace_thread, i, nx, ny, comp, numsamples, &cam, world, p);
  }

  //Join the threads with the main thread
  for (int i = 0; i < num_threads; ++i) {
    threads[i].join();
  }

//  output_ppm(nx, ny, data);
  write_to_png(nx, ny, comp, data);

  delete[] threads;
  delete[] data;
  return 0;
}