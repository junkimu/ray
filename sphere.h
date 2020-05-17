#pragma once

#include "hittable.h"

class sphere: public hittable {
    public:
        sphere() {}
        sphere(vec3 center, float r): m_center(center), m_radius(r) {}
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
        vec3 m_center;
        float m_radius;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
  vec3 oc = r.origin() - m_center;
  float a = dot(r.direction(), r.direction());
  float b = dot(oc, r.direction());
  float c = dot(oc, oc) - m_radius * m_radius;
  float discriminant = b * b - a * c;
  if (discriminant > 0) {
      float temp = (-b - sqrt(discriminant)) / a;
      if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
          rec.normal = (rec.p - m_center) / m_radius;
            return true;
      }
      temp = (-b + sqrt(discriminant)) / a;
      if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
          rec.normal = (rec.p - m_center) / m_radius;
            return true;
      }
  }
  return false;
}
