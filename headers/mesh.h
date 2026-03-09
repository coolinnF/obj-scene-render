#ifndef MESHH
#define MESHH

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "hitable.h"
#include "bvh.h"

// TRIANGLE PRIMITIVE
// Three vertices: face normal computed from the cross product.
// Sharp edges everywhere, fine for arch vis
class triangle : public hitable
{
public:
    vec3 v0, v1, v2;
    vec3 face_normal;   // pre-computed, unit length
    material *mat_ptr;

    triangle(vec3 a, vec3 b, vec3 c, material *m): v0(a), v1(b), v2(c), mat_ptr(m)
    {
        face_normal = unit_vector(cross(v1 - v0, v2 - v0));
    }

    // Moller–Trumbore intersection
    virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const override
    {
        const float EPSILON = 1e-7f;

        vec3  edge1 = v1 - v0;
        vec3  edge2 = v2 - v0;
        vec3  h     = cross(r.direction(), edge2);
        float a     = dot(edge1, h);

        if (a > -EPSILON && a < EPSILON)
            return false;

        float f = 1.0f / a;
        vec3  s = r.origin() - v0;
        float u = f * dot(s, h);

        if (u < 0.0f || u > 1.0f)
            return false;

        vec3  q = cross(s, edge1);
        float v = f * dot(r.direction(), q);

        if (v < 0.0f || u + v > 1.0f)
            return false;

        float t = f * dot(edge2, q);

        if (t < tmin || t > tmax)
            return false;

        rec.t       = t;
        rec.p       = r.point_at_parameter(t);
        rec.normal  = face_normal;
        rec.mat_ptr = mat_ptr;
        return true;
    }

    // Tight AABB: small pad prevents zero-thickness slabs on axis-aligned tris
    virtual bool bounding_box(aabb &box) const override
    {
        const float PAD = 1e-4f;
        box = aabb(
            vec3(fmin(fmin(v0.x(), v1.x()), v2.x()) - PAD,
                 fmin(fmin(v0.y(), v1.y()), v2.y()) - PAD,
                 fmin(fmin(v0.z(), v1.z()), v2.z()) - PAD),
            vec3(fmax(fmax(v0.x(), v1.x()), v2.x()) + PAD,
                 fmax(fmax(v0.y(), v1.y()), v2.y()) + PAD,
                 fmax(fmax(v0.z(), v1.z()), v2.z()) + PAD));
        return true;
    }
};


// OBJ LOADER
int load_obj(const std::string &path,
             material *mat,
             std::vector<hitable *> &out_list)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "[OBJ] Cannot open: " << path << "\n";
        return -1;
    }

    std::vector<vec3> positions;
    int tri_count = 0;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream ss(line);
        std::string token;
        ss >> token;

        // vertex position
        if (token == "v")
        {
            float x, y, z;
            ss >> x >> y >> z;
            positions.push_back(vec3(x, y, z));
        }
        // face:
        else if (token == "f")
        {
            std::vector<int> vi;

            std::string word;
            while (ss >> word)
            {
                size_t slash = word.find('/');
                int idx = std::stoi(slash == std::string::npos
                                    ? word
                                    : word.substr(0, slash));
                vi.push_back(idx);
            }

            // Fan triangulation from vi[0]
            for (int i = 1; i + 1 < (int)vi.size(); i++)
            {
                out_list.push_back(new triangle(
                    positions[vi[0]     - 1],
                    positions[vi[i]     - 1],
                    positions[vi[i + 1] - 1],
                    mat));
                tri_count++;
            }
        }
        // vn, vt, mtllib, usemtl, s, g, o — silently ignored
    }

    std::cerr << "[OBJ] Loaded " << tri_count
              << " triangles from " << path << "\n";
    return tri_count;
}

#endif