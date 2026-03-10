/// main.cpp ///////////////////
// Colin Fairborn
// Modified: 3/10/26 (comments)
// References:
//      - Peter Shirley, "Ray Tracing in One Weekend", 2018
//        https://www.realtimerendering.com/raytracing/Ray%20Tracing%20in%20a%20Weekend.pdf
// Purpose:
//      - Build the scene (OBJ meshes and/or spheres, materials)
//      - Wrap everything in a BVH for faster rendering
//      - Render by shooting 'ns' rays per pixel and taking the avg.
//      - Output a PPM image to stdout 
// Usage: 
//      g++ ./source/main.cpp -o raytracer
//      ./raytracer.exe > output.ppm
///////////////////////////////

#include <iostream>
#include <ctime>
#include "float.h"

#include "../headers/bvh.h"
#include "../headers/camera.h"
#include "../headers/materials.h"
#include "../headers/sphere.h"
#include "../headers/mesh.h"

/////////////////////////////////////////////
// Color - recursive path-tracing shading
////
// Inputs: r (ray to trace), world (BVH), depth (current recursion depth)
////
// Purpose:
//  Traces a ray through the scene and returns approximate radiance.
//  On a hit, the surface scatters the ray: recurse until ray escapes
//  (sky return) or if its absorbed (black).
vec3 color(const ray &r, hitable *world, int depth)
{
    hit_record rec;

    // t_min = 0.001 avoids "shadow-acne"
    if (world->hit(r, 0.001, FLT_MAX, rec))
    {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        {
            // Multiply incoming radiance by the surface's color attenuation
            return attenuation * color(scattered, world, depth + 1);
        }
        else
        {
            // Max depth: return black
            return vec3(0, 0, 0);
        }
    }
    else
    {
        // Sky background
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5f * (unit_direction.y() + 1.0f);

        // Sunset gradient to look prettier
        vec3 horizon(1.0, 0.45, 0.15);   // warm orange
        vec3 zenith (0.10, 0.15, 0.40);  // deep dusk blue
        vec3 ground (0.20, 0.12, 0.08);  // dark warm ground

        if (t > 0.5f) {
            // Horizon -> zenith
            float s = (t - 0.5f) * 2.0f;
            return (1.0f - s) * horizon + s * zenith;
        } else {
            // Ground -> horizon
            float s = t * 2.0f;
            return (1.0f - s) * ground + s * horizon;
        }
    }
}

/////////////////////////////////////////////
// Main
int main()
{
    srand(std::time(NULL)); // Seed RNG

    // Image dimenstions and samples 
    int nx = 300; // Width
    int ny = 150; // Height
    int ns = 250; // Samples / px
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";

    std::vector<hitable*> scene;

    // // spheres
    // scene.push_back(new sphere(vec3(0,    0,   -1),   0.5,  new lambertian(vec3(0.1, 0.2, 0.5))));
    // scene.push_back(new sphere(vec3(0, -100.5, -1),  100,   new lambertian(vec3(0.8, 0.8, 0.0))));
    // scene.push_back(new sphere(vec3(1,    0,   -1),   0.5,  new metal(vec3(0.8, 0.6, 0.2), 1)));

    scene.push_back(new sphere(vec3(-1,   1,   3),   0.5,  new dielectric(1.5)));
    scene.push_back(new sphere(vec3(-1,   1,   3),  -0.45, new dielectric(1.5)));

    // Load an .obj
    material *wall   = new lambertian(vec3(0.76, 0.70, 0.62));  // warm off-white plaster
    material *table  = new metal(vec3(0.60, 0.55, 0.50), 0.35); // brushed steel
    material *mirror = new metal(vec3(0.60, 0.55, 0.50), 0.00); // Polished
    material *glass  = new dielectric(1.5);                     // window glass (IOR 1.5)
    material *plant  = new lambertian(vec3(0.18, 0.42, 0.18));  // matte green leaves
    material *pot    = new lambertian(vec3(0.78, 0.35, 0.20));  // terrracotta

    load_obj("./assets/room.obj",   wall,  scene);
    load_obj("./assets/window.obj", glass, scene);
    load_obj("./assets/table.obj",  table, scene);
    load_obj("./assets/pot.obj",    pot,   scene);
    load_obj("./assets/plant.obj",  plant, scene);
    load_obj("./assets/mirror.obj", mirror, scene);

    // BVH over everything — spheres and triangles together
    hitable *world = new bvh_node(scene.data(), (int)scene.size());

    // Camera properties
    vec3 lookfrom(-0.9, 1.1, 0.85); // About eye level in scene
    vec3 lookat( 0.4, 0.85, -0.3);  
    vec3 vup(0, 1, 0);
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 0.015;

    // Camera
    camera cam(lookfrom, lookat, vup, 50, float(nx) / float(ny), aperture, dist_to_focus);

    // Render loop. Eeeevery pixel.
    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            vec3 col(0, 0, 0);
            for (int s = 0; s < ns; s++)
            {
                float u = float(i + (float)rand() / RAND_MAX) / float(nx);
                float v = float(j + (float)rand() / RAND_MAX) / float(ny);

                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += color(r, world, 0);
            }
            
            // Avg the samples
            col /= float(ns);

            // Gamma correction. Simple gamma-2 encode.
            // Makes sure scene isn't too dark.
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

            // Convert [0,1]f to [0,255]int for PPM output
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
