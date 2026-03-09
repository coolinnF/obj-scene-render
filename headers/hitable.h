#ifndef HITABLEH
#define HITABLEH
#include "ray.h"

// forward declarations so hitable can ref
class aabb;
class material;

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    material *mat_ptr;
};

class hitable {
    public:
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
        virtual bool bounding_box(aabb& box) const = 0;
};

#endif