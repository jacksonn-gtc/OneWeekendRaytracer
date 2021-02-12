#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "hittable.h"
// won't compile for me unless include hittable.h
// the github and online example show only a forward declaration of hit_record
// see explanation section 9 for details

//struct hit_record;

class material {
    public:
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const = 0;
};

// Lambertian Perfect diffuser; looks the same from any angle, scatters light equally 
// Approximate by taking finite number of random scatters
class lambertian : public material {
    public:
        lambertian(const color& a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            auto scatter_direction = rec.normal + random_unit_vector(); // pick our random direction
            //auto scatter_direction = rec.p + random_in_hemisphere(rec.normal);    // Intuitive method
            //auto scatter_direction = rec.p + rec.normal + random_unit_vector();   // Shirley's lambert approx.

            // Catch degenerate scatter direction
            if (scatter_direction.near_zero()) {
                scatter_direction = rec.normal;
            }

            scattered = ray(rec.p, scatter_direction);  // cast a ray from rec.p in the direction
            attenuation = albedo;   // give the color of this material
            return true;
        }

    public:
        color albedo;
};

// Mirror reflector; Scatters light at same angle from normal as incoming light
// Use a formula to calculate reflection, use fuzz to pick fuzzy (fuzz>0) or perfect (fuzz=0) mirror reflection
class metal : public material {
    public:
        metal(const color& a, double f) : albedo(a), fuzz(f) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);    // reflect our ray along the surface's normal vector
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());   // randomly 'fuzz' our new ray slightly
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }

    public:
        color albedo;
        double fuzz;
};

// Refractive; Allows light to travel through the object
// Calculate angle of refraction by Snell's Law
class dielectric : public material {
    public:
        dielectric(double index_of_refraction) : ir(index_of_refraction) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            attenuation = color(1.0, 1.0, 1.0);
            double refraction_ratio = rec.front_face ? (1.0/ir) : ir;   // Snell's Law refraction ratio component n1/n2, 1.0 is for air

            vec3 unit_direction = unit_vector(r_in.direction());
            vec3 refracted = refract(unit_direction, rec.normal, refraction_ratio); // Calculate our direction by Snell's Law

            scattered = ray(rec.p, refracted);
            return true;
        }

    public:
        double ir;
};

#endif