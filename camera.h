#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

class camera {
    public:
        camera(
            point3 lookfrom,
            point3 lookat,
            vec3   vup,
            double vfov, // vertical field-of-view in degrees
            double aspect_ratio,
            double aperture,
            double focus_dist
        ) {
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;

            //std::cerr << "viewport_height:" << viewport_height << ", viewport_width:" << viewport_width << '\n';
            
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);
            
            //std::cerr << "w:" << w << ", u:" << u << ", v:" << v <<'\n';
            
            origin = lookfrom;
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w;

            //std::cerr << "origin:" << origin << ", horiz:" << horizontal/2 << ", vert:" << vertical/2 << ", low_left:" << lower_left_corner << '\n';

            lens_radius = aperture / 2;
        }

        ray get_ray(double s, double t) const {
            vec3 rd = lens_radius * random_in_unit_disk();  // pick a random point for our sample to start from
            vec3 offset = u * rd.x() + v * rd.y();

            return ray(
                origin + offset, 
                lower_left_corner + s*horizontal + t*vertical - origin - offset
            );
        }
    
    public:
        point3 origin;
        vec3 horizontal;
        vec3 vertical;
        point3 lower_left_corner;
        vec3 w,u,v;
        double lens_radius;
};

#endif