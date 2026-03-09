#ifndef HITABLELISTH
#define HITABLELISTH

#include "hitable.h"

class hitable_list : public hitable
{
public:
    hitable_list() {}
    hitable_list(hitable **l, int n)
    {
        list = l;
        list_size = n;
    }
    virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const;
    virtual bool bounding_box(aabb &box) const;
    hitable **list;
    int list_size;
};

#endif