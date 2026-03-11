/// bvh.h /////////////////////
// Colin Fairborn
// Modified: 3/10/26 (comments)
// References:
//      - Peter Shirley, "Ray Tracing in One Weekend", 2018
//        https://www.realtimerendering.com/raytracing/Ray%20Tracing%20in%20a%20Weekend.pdf
// Purpose:
//      - Builds a binary tree over hitables
//      - Each node has an AABB enclosing its children
//      - During taversal, nodes whose AABB is missed by the ray get skipped
//      - O(n) reduced to O(log n)
///////////////////////////////

#ifndef BVHH
#define BVHH

#include "hitable.h"
#include "hitablelist.h"

// Axis aligned bounding box
// Just two points. Min corner and max corner of the box
class aabb
{
public:
    // default constructor
    aabb() {}

    // constructor that takes two vec3s: min corner and max corner
    aabb(const vec3 &min_c_in, const vec3 &max_c_in)
    {
        min_cor = min_c_in;
        max_cor = max_c_in;
    }
    // getters for min and max
    vec3 max() { return max_cor; }
    vec3 min() { return min_cor; }

    // Slab method. Like binary search for rays hitting bounding boxes of obs
    bool hit(const ray &r, float tmin, float tmax) const
    {
        for (int a = 0; a < 3; a++)
        { // loop through axes
            float invD = float(1.0) / r.direction()[a];
            float t0 = (min_cor[a] - r.origin()[a]) * invD;
            float t1 = (max_cor[a] - r.origin()[a]) * invD;

            // if rays going negative, swap
            if (invD < 0.0f)
                std::swap(t0, t1);

            tmin = t0 > tmin ? t0 : tmin; // latest entry pt
            tmax = t1 < tmax ? t1 : tmax; // earliest entry pt

            if (tmax <= tmin)
                return false; // Ray missed objs's BB
        }
        return true; // hit!!
    }

    vec3 min_cor;
    vec3 max_cor;
};

// Given two boxes, return one big box that contains both
// just take the min of all mins and max of all maxes
aabb surrounding_box(aabb box0, aabb box1)
{
    vec3 small_box(
        fmin(box0.min().x(), box1.min().x()),
        fmin(box0.min().y(), box1.min().y()),
        fmin(box0.min().z(), box1.min().z()));
    vec3 big_box(
        fmax(box0.max().x(), box1.max().x()),
        fmax(box0.max().y(), box1.max().y()),
        fmax(box0.max().z(), box1.max().z()));
    return aabb(small_box, big_box);
}

bool hitable_list::bounding_box(aabb &box) const
{
    if (list_size < 1)
        return false;
    aabb temp_box;
    if (!list[0]->bounding_box(temp_box))
        return false;
    box = temp_box;
    for (int i = 1; i < list_size; i++)
    {
        if (list[i]->bounding_box(temp_box))
            box = surrounding_box(box, temp_box);
        else
            return false;
    }
    return true;
}

// Insertion sort along a given axis
void sort_hitables(hitable **list, int n, int axis)
{
    for (int i = 1; i < n; i++)
    {
        hitable *key = list[i];
        aabb key_box;
        key->bounding_box(key_box);
        float key_val = key_box.min()[axis];

        int j = i - 1;
        while (j >= 0)
        {
            aabb box;
            list[j]->bounding_box(box);
            if (box.min()[axis] <= key_val)
                break;
            list[j + 1] = list[j];
            j--;
        }
        list[j + 1] = key;
    }
}

// BVH NODE
class bvh_node : public hitable
{
public:
    bvh_node() {}
    bvh_node(hitable **l, int n);

    virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const;
    virtual bool bounding_box(aabb &b) const;

    hitable *left;
    hitable *right;
    aabb box;
};

bvh_node::bvh_node(hitable **l, int n)
{
    // pick random axis to split on
    int axis = int(3 * (float)rand() / RAND_MAX);

    sort_hitables(l, n, axis);

    if (n == 1)
    {
        left = right = l[0];
    }
    else if (n == 2)
    {
        left = l[0];
        right = l[1];
    }
    else
    {
        left = new bvh_node(l, n / 2);
        right = new bvh_node(l + n / 2, n - n / 2);
    }

    aabb box_left, box_right;
    if (!left->bounding_box(box_left) || !right->bounding_box(box_right))
        std::cerr << "No bounding box in bvh_node constructor\n";

    box = surrounding_box(box_left, box_right);
}

bool bvh_node::hit(const ray &r, float tmin, float tmax, hit_record &rec) const
{
    if (box.hit(r, tmin, tmax))
    {
        hit_record left_rec, right_rec;
        bool hit_left = left->hit(r, tmin, tmax, left_rec);
        bool hit_right = right->hit(r, tmin, tmax, right_rec);

        if (hit_left && hit_right)
        {
            rec = left_rec.t < right_rec.t ? left_rec : right_rec;
            return true;
        }
        else if (hit_left)
        {
            rec = left_rec;
            return true;
        }
        else if (hit_right)
        {
            rec = right_rec;
            return true;
        }
    }
    return false;
}

bool bvh_node::bounding_box(aabb &b) const
{
    b = box;
    return true;
}

#endif