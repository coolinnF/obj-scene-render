#include <iostream>
#include <ctime>
#include "float.h"

#include "../headers/bvh.h"
#include "../headers/camera.h"
#include "../headers/materials.h"
#include "../headers/sphere.h"

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation*color(scattered, world, depth+1);
        }
        else {
            return vec3(0, 0, 0);
        }
    }
    else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

int main() {
    srand(std::time(NULL));
    int nx = 300;
    int ny = 150;
    int ns = 100;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";

    // Object initialization into scene
    hitable *list[5];
    list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
    list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 1));
    list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
    list[4] = new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5));
    hitable *world = new bvh_node(list, 5);

    // Camera propertiesl
    vec3 lookfrom(3, 3, 2);
    vec3 lookat(0, 0, -1);
    vec3 vup(0, 1, 0);
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 0.1;

    // Camera
    camera cam(lookfrom, lookat, vup, 30, float(nx)/float(ny), aperture, dist_to_focus);

    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s = 0; s < ns; s++) {
                float u = float(i + (float)rand() / RAND_MAX) / float(nx);
                float v = float(j + (float)rand() / RAND_MAX) / float(ny);

                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += color(r, world, 0);
            }
            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2])); // Less dark of shadow
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
