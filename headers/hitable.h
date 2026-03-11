/// hitable.h /////////////////////
// Colin Fairborn
// Modified: 3/11/26 (comments)
// References:
//      - Peter Shirley, "Ray Tracing in One Weekend", 2018
//        https://www.realtimerendering.com/raytracing/Ray%20Tracing%20in%20a%20Weekend.pdf
// Purpose:
//      - Abstract base class for anything a ray can intersect
//      - hit_record bundles all intersection data so callers dont need
//        separate out params for everything
///////////////////////////////

#ifndef HITABLEH
#define HITABLEH
#include "ray.h"

// forward declarations so hitable can ref
class aabb;
class material;

// Stores the result of a successful ray–object intersection
struct hit_record
{
    float t;            // Ray param at the hit point
    vec3 p;             // World space pos of the hit
    vec3 normal;        // Surface normat at the hit
    material *mat_ptr;  // Material at the hit
};

class hitable
{
public:
    // Test whether r hit object in [t_min, t_max]
    // On hit fills rec and returns true otherwise false
    virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const = 0;
    
    // Compute AABB enclosing object
    // Returns true and fills box on success. False if no real bounds
    virtual bool bounding_box(aabb &box) const = 0;
};

#endif