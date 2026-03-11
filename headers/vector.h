/// vector.h /////////////////////
// Colin Fairborn
// Modified: 3/11/26 (comments)
// References:
//      - Peter Shirley, "Ray Tracing in One Weekend", 2018
//        https://www.realtimerendering.com/raytracing/Ray%20Tracing%20in%20a%20Weekend.pdf
// Purpose:
//      - Core 3-component float vector type used everywhere in the renderer
//      - Covers positions, directions, colors (r,g,b aliased to x,y,z)
//      - All operators inlined for performance
///////////////////////////////

#define _USE_MATH_DEFINES
#include <iostream>
#include <math.h>
#include <stdlib.h>

class vec3
{
public:
    // Default constructor. Leaves everything unitialized for starts
    vec3() {}

    // Construct from three floats
    vec3(float e0, float e1, float e2)
    {
        e[0] = e0;
        e[1] = e1;
        e[2] = e2;
    }

    // Component getters
    //// Positional
    inline float x() const { return e[0]; }
    inline float y() const { return e[1]; }
    inline float z() const { return e[2]; }
    //// Color
    inline float r() const { return e[0]; }
    inline float g() const { return e[1]; }
    inline float b() const { return e[2]; }

    // Unary ops
    inline const vec3 &operator+() const { return *this; }  // Noop positive
    inline vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); } // Negative
    
    // Array type access
    inline float operator[](int i) const { return e[i]; }
    inline float &operator[](int i) { return e[i]; };

    // Arithmetic op declarations
    inline vec3 &operator+=(const vec3 &v2);
    inline vec3 &operator-=(const vec3 &v2);
    inline vec3 &operator*=(const vec3 &v2);
    inline vec3 &operator/=(const vec3 &v2);
    inline vec3 &operator*=(const float t);
    inline vec3 &operator/=(const float t);

    // Euclidean length
    inline float length() const
    {
        return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
    }

    // Sqared length. Cheaper for comparisons and such
    inline float squared_length() const
    {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    // Normalize vector
    inline void make_unit_vector();

    float e[3];
};

// Stream ops
inline std::istream &operator>>(std::istream &is, vec3 &t)
{
    is >> t.e[0] >> t.e[1] >> t.e[2];
    return is;
}

inline std::ostream &operator<<(std::ostream &is, const vec3 &t)
{
    is << t.e[0] << t.e[1] << t.e[2];
    return is;
}

// In place normalizaton
inline void vec3::make_unit_vector()
{
    float k = 1.0 / sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
    e[0] *= k;
    e[1] *= k;
    e[2] *= k;
}

// Binary arith
inline vec3 operator+(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline vec3 operator-(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

// Multiply (color * color)
inline vec3 operator*(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

// Divide components
inline vec3 operator/(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

// Scalar multiply (t*v or v*t)
inline vec3 operator*(float t, const vec3 &v)
{
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}
inline vec3 operator*(const vec3 &v, float t)
{
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

// Scalar divide
inline vec3 operator/(const vec3 &v, float t)
{
    return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

// Vector math stuff.
// Dot product. Used a tonnnn.
inline float dot(const vec3 &v1, const vec3 &v2)
{
    return ((v1.e[0] * v2.e[0]) + (v1.e[1] * v2.e[1]) + (v1.e[2] * v2.e[2]));
}

// Cross product. Face normals and camera basis vects.
inline vec3 cross(const vec3 &v1, const vec3 &v2)
{
    return vec3((v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
                (-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
                (v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
}


// In place operator defs
inline vec3 &vec3::operator+=(const vec3 &v)
{
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
}

inline vec3 &vec3::operator*=(const vec3 &v)
{
    e[0] *= v.e[0];
    e[1] *= v.e[1];
    e[2] *= v.e[2];
    return *this;
}

inline vec3 &vec3::operator/=(const vec3 &v)
{
    e[0] /= v.e[0];
    e[1] /= v.e[1];
    e[2] /= v.e[2];
    return *this;
}

inline vec3 &vec3::operator-=(const vec3 &v)
{
    e[0] -= v.e[0];
    e[1] -= v.e[1];
    e[2] -= v.e[2];
    return *this;
}

inline vec3 &vec3::operator*=(const float t)
{
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
}

inline vec3 &vec3::operator/=(const float t)
{
    float k = 1.0 / t; // one division instead of 3!!

    e[0] *= k;
    e[1] *= k;
    e[2] *= k;
    return *this;
}

// Return unit length copy
inline vec3 unit_vector(vec3 v)
{
    return (v / (v.length()));
}