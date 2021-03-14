#ifdef _MSC_VER 
#ifdef _CRT_SECURE_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#endif
#define _CRT_SECURE_NO_WARNINGS 1
#endif

//#define STB_IMAGE_WRITE_IMPLEMENTATION
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

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
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

  hittable_list world = random_scene();
  point3 lookfrom(13,2,3);
  point3 lookat(0,0,0);
  vec3 vup(0,1,0);
  auto dist_to_focus = 10.0;
  auto aperture = 0.1;

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