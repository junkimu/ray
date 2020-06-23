#ifdef _MSC_VER 
#ifdef _CRT_SECURE_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#endif
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#define STB_IMAGE_WRITE_IMPLEMENTATION
#ifdef _MSC_VER
#define STBI_MSC_SECURE_CRT
#endif

#include "rtweekend.h"
#include "color.h"
#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "triangle.h"
#include <iostream>
#include "stb_image_write.h"
#include <thread>

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

color ray_color(const ray& r, hittable* world, int depth) {
    float EPSILON = 0.001f; //to ignore hits very near zero
    hit_record rec;
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    if (world->hit(r, EPSILON, infinity, rec)) {
    //    return 0.5 * vec3(rec.normal.x()+1, rec.normal.y()+1, rec.normal.z()+1); //visualize surface normal
      ray scattered;
      color attenuation;
      if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return attenuation * ray_color(scattered, world, depth-1);
      return color(0.0f, 0.0f, 0.0f);
    }
    else {
      vec3 unit_direction = unit_vector(r.direction());
      float t = 0.5f * (unit_direction.y() + 1.0f); // -1.0 ~ 1.0 -> 0.0 ~ 1.0
      return (1.0f - t) * color(1.0f, 1.0f, 1.0f) + t * color(0.5f, 0.7f, 1.0f); //lerp between white and blue
    }
}

static int num_threads = 1;

void raytrace_thread(int thread_id, int nx, int ny, int comp, int numsamples, camera* cam, hittable* world, unsigned char* p, int maxdepth) {
  for (int j = ny - 1; j >= 0; j--) {
    if (j % num_threads != thread_id) {
      p += nx * comp; // go to next row
      continue;
    }
    for (int i = 0 ;i < nx; i++ ) {
      vec3 col(0,0,0);
      for (int s = 0; s < numsamples; s++) {
        float u = float(i + random_float()) / float(nx);
        float v = float(j + random_float()) / float(ny);
        ray r = cam->get_ray(u,v);
        col += ray_color(r, world, maxdepth);
      }
      col /= float(numsamples);
      col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2])); //gamma correct
      int ir = static_cast<int>(256 * clamp(col[0], 0.0f, 0.999f));
      int ig = static_cast<int>(256 * clamp(col[1], 0.0f, 0.999f));
      int ib = static_cast<int>(256 * clamp(col[2], 0.0f, 0.999f));
      *p++ = (unsigned char)ir;
      *p++ = (unsigned char)ig;
      *p++ = (unsigned char)ib;
    }
  }
}


int main( int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  const char* numcpus_env = getenv("NUMBER_OF_PROCESSORS"); //Windows
  if (numcpus_env) {
    num_threads = atoi(numcpus_env);
  }
  std::cerr << "number of cpus detected:" << num_threads << '\n';

  const auto aspect_ratio = 16.0f / 9.0f;
  int nx = 400;
  int ny = static_cast<int>(nx / aspect_ratio);
  int numsamples = 100;
  int comp = 3; //RGB
  unsigned char* data = new unsigned char[nx * ny * comp];
  unsigned char* p = data;
  const int max_depth = 50;

  hittable_list world;
  world.add(make_shared<sphere>(
    point3(0.0f, 0.0f ,-1.0f), 0.5f, make_shared<lambertian>(color(0.1f, 0.2f, 0.5f))));
  world.add(make_shared<sphere>(
    point3(0.0f,-100.5f,-1.0f), 100.0f, make_shared<lambertian>(color(0.8f, 0.8f, 0.0f))));
  world.add(make_shared<sphere>(
    point3(1.0f,0.0f,-1.0f), 0.5f, make_shared<metal>(color(0.8f, 0.6f, 0.2f), 0.3f)));
  world.add(make_shared<sphere>(
    point3(-1.0f,0.0f,-1.0f), 0.5f, make_shared<dielectric>(1.5f)));
  world.add(make_shared<sphere>(
    point3(-1.0f,0.0f,-1.0f), -0.45f, make_shared<dielectric>(1.5f)));

/*
  world.add(make_shared<triangle>(
    vec3(0,-0.5,0), vec3(0,-0.5,-2), vec3(1,-0.5,0), vec3(0,1,0),make_shared<lambertian>(color(0.3f, 0.3f, 0.7f))));
  world.add(make_shared<triangle>(
    vec3(0,-0.5,0), vec3(0,-0.5,-2), vec3(-1,-0.5,0), vec3(0,1,0),make_shared<metal>(color(0.3f, 0.3f, 0.7f),0.0f)));
*/
  point3 lookfrom(3,3,2);
  point3 lookat(0,0,-1);
  vec3 vup(0,1,0);
  auto dist_to_focus = (lookfrom-lookat).length();
  auto aperture = 2.0;
  camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

  std::cerr << "spawning " << num_threads << " threads \n";
  std::thread *threads = new std::thread[num_threads];
  for (int i = 0; i < num_threads; ++i) {
    threads[i] = std::thread(raytrace_thread, i, nx, ny, comp, numsamples, &cam, &world, p, max_depth);
  }

  std::cerr << "waiting for " << num_threads << " threads \n";
  //Join the threads with the main thread
  for (int i = 0; i < num_threads; ++i) {
    threads[i].join();
  }

  std::cerr << "writing results\n";
//  output_ppm(nx, ny, data);
  write_to_png(nx, ny, comp, data);

  delete[] threads;
  delete[] data;
  return 0;
}