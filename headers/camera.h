/// camera.h /////////////////////
// Colin Fairborn
// Modified: 3/10/26 (comments)
// References:
//      - Peter Shirley, "Ray Tracing in One Weekend", 2018
//        https://www.realtimerendering.com/raytracing/Ray%20Tracing%20in%20a%20Weekend.pdf
// Purpose:
//      - Perspective camera with DoF support
//      - Rays are jittered across a disk of radius lens_radius to simulate blur;
//        aperture = 0 gives a perfect pinhole (no DoF)
///////////////////////////////

#ifndef CAMERAH
#define CAMERAH

#include "ray.h"

// Returns a random point inside a unit disk (lens)
// Jitters ray origins to simulate blur for DoF
vec3 random_in_unit_disk()
{
    vec3 p;
    do
    {
        // Map [0,1)^2 to [-1,1]^2, keep z=0
        p = 2.0 * vec3(
                      (float)rand() / RAND_MAX,
                      (float)rand() / RAND_MAX,
                      0) -
            vec3(1, 1, 0);
    } while (dot(p, p) >= 1.0); // Reject points outside of circle
    return p;
}

class camera
{
public:
    // lookfrom:   cam position
    // lookat:     point the cams aimed at
    // vup:        up vector
    // vfov:       vertical FOV in degs
    // aspect:     image width / height
    // aperture:   lens diameter; 0 = pinhole (no blur!)
    // focus_dist: distance from lens to focal plane
    camera(
        vec3 lookfrom,
        vec3 lookat,
        vec3 vup,
        float vfov,     // vertical fov in degrees
        float aspect,   // nx/ny
        float aperture, // 0 = no blur
        float focus_dist)
    {
        lens_radius = aperture / 2;

        // Figure out how tall and wide the screen is based on field of view
        float theta = vfov *  M_PI / 180;
        float half_height = tan(theta / 2);
        float half_width = aspect * half_height;

        origin = lookfrom;

        // build orthonormal basis
        w = unit_vector(lookfrom - lookat); // backward
        u = unit_vector(cross(vup, w)); // right
        v = cross(w, u); // up

        // Put screen in the scene at focus distance
        lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v - focus_dist * w;

        horizontal = 2 * half_width * focus_dist * u;
        vertical = 2 * half_height * focus_dist * v;
    }

    // Shoot a ray through screen position (s, t)
    ray get_ray(float s, float t)
    {
        // randomize a tiny for blur
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();
        return ray(
            origin + offset,
            lower_left_corner + s * horizontal + t * vertical - origin - offset);
    }

    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    float lens_radius;
};

#endif