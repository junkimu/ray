#pragma once

#include "ray.h"

class camera {
    public:
        camera():
            m_origin(0.0, 0.0, 0.0),
            m_lowerleft(-2.0, -1.0, -1.0),
            m_horizontal(4.0, 0.0, 0.0),
            m_vertical(0.0, 2.0, 0.0) {}
        ray get_ray(float x, float y) {return ray(m_origin, m_lowerleft + x * m_horizontal + y * m_vertical - m_origin);}

        vec3 m_origin;
        vec3 m_lowerleft;
        vec3 m_horizontal;
        vec3 m_vertical;
};