#pragma once

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <random>


// Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180;
}

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0); // [0.0, 1.0)
    static thread_local std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

inline float random_float() {
    return static_cast<float>(random_double());
}

inline float random_float(float min, float max) {
    return static_cast<float>(random_double(min, max));
}

inline double clamp(float x, float min, float max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

// Common Headers

#include "ray.h"
#include "vec3.h"
