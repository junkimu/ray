#pragma once

class material {
    public:
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const = 0;
};

class lambertian: public material {
    public:
        lambertian(const color& a) : albedo(a) {}

        virtual bool scatter(
            const ray&, const hit_record& rec, color& attenuation, ray& scattered
        ) const {
            vec3 scatter_direction = random_in_hemisphere(rec.normal);
            scattered = ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }

    public:
        color albedo;
};

class metal: public material {
    public:
        metal(const color&a, float f) : albedo(a), fuzz(f < 1.0f ? f : 1.0f) {}
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }

    public:
        color albedo;
        float fuzz;
};