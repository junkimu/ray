#pragma once

#include "hittable.h"

class triangle: public hittable {
    public:
        triangle() {}
        triangle(vec3 v0, vec3 v1, vec3 v2, vec3 normal): m_vertex0(v0), m_vertex1(v1), m_vertex2(v2), m_normal(normal) {
            m_normal.make_unit_vector();
        }
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
        vec3 m_vertex0;
        vec3 m_vertex1;
        vec3 m_vertex2;
        vec3 m_normal;
    private:
        bool  moller_trumbore(const ray& r, float* t_out) const;
};

bool triangle::moller_trumbore(const ray& r, float* t_out) const {
    const float EPSILON = 0.0000001f;
    vec3 edge1 = m_vertex1 - m_vertex0;
    vec3 edge2 = m_vertex2 - m_vertex0;
    vec3 h, s, q;
    float a, f, u, v;
    
    
    h = cross(r.direction(), edge2);
    a = dot(edge1, h);
    if (a > -EPSILON && a < EPSILON) {
        return false;    // This ray is parallel to this triangle.
    }
    f = 1.0f / a;
    s = r.origin() - m_vertex0;
    u = f * dot(s,h);
    if (u < 0.0 || u > 1.0) {
        return false;
    }
    q = cross(s, edge1);
    v = f * dot(r.direction(), q);
    if (v < 0.0 || u + v > 1.0) {
        return false;
    }
    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * dot(edge2, q);
    if (t > EPSILON) // ray intersection
    {
        *t_out = t;
        return true;
    } 
    else // This means that there is a line intersection but not a ray intersection.
    {
        return false;
    }
}

bool triangle::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    float t;
    if (moller_trumbore(r, &t)) {
        if (t < t_max && t > t_min) {
            rec.t = t;
            rec.p = r.at(rec.t);
            rec.normal = m_normal;
            return true;
        }
    }
    return false;
}
