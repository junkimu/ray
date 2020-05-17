#pragma once

#include "hittable.h"

class sphere: public hittable {
    public:
        sphere() {}
        sphere(point3 center, float r): center(center), radius(r) {}
        virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
        point3 center;
        float radius;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
  vec3 oc = r.origin() - center;
  float a = dot(r.direction(), r.direction());
  float b = dot(oc, r.direction());
  float c = dot(oc, oc) - radius * radius;
  float discriminant = b * b - a * c;
  if (discriminant > 0) {
      float temp = (-b - sqrt(discriminant)) / a;
      if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            return true;
        }
      temp = (-b + sqrt(discriminant)) / a;
      if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            return true;
      }
  }
  return false;
}
