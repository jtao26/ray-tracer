#include "Point3.h"
#include "Vector3.h"
#include "Sphere.h"

#include <math.h>

#define MATH_DIV_OFFSET 1e-8

Point3::Point3() {
    p[0] = 0.0;
    p[1] = 0.0;
    p[2] = 0.0;
}

Point3::Point3(float x, float y, float z) {
    p[0] = x;
    p[1] = y;
    p[2] = z;
}

Point3::Point3(float* ptr) {
    p[0] = ptr[0];
    p[1] = ptr[1];
    p[2] = ptr[2];
}

Point3::Point3(const Point3& other) {
    p[0] = other[0];
    p[1] = other[1];
    p[2] = other[2];
}

Point3::~Point3() {
}

bool Point3::operator==(const Point3& other) const {
    return (fabs(other[0] - p[0]) < MATH_DIV_OFFSET &&
        fabs(other[1] - p[1]) < MATH_DIV_OFFSET &&
        fabs(other[2] - p[2]) < MATH_DIV_OFFSET);
}

bool Point3::operator!=(const Point3& other) const {
    return (fabs(other[0] - p[0]) >= MATH_DIV_OFFSET ||
        fabs(other[1] - p[1]) >= MATH_DIV_OFFSET ||
        fabs(other[2] - p[2]) >= MATH_DIV_OFFSET);
}

Point3& Point3::operator=(const Point3& other) {
    p[0] = other[0];
    p[1] = other[1];
    p[2] = other[2];
    return *this;
}

float Point3::operator[](const int i) const {
    if ((i >= 0) && (i <= 2)) {
        return p[i];
    }
    else {
        // w component of a point is 1 so return the constant 1.0
        return 1.0;
    }
}

float& Point3::operator[](const int i) {
    return p[i];
}

static const Point3 s_zerop3d = Point3(0, 0, 0);

static const Point3 s_onep3d = Point3(1, 1, 1);

const Point3& Point3::Origin() { return s_zerop3d; }

const Point3& Point3::Zero() { return s_zerop3d; }

const Point3& Point3::One() { return s_onep3d; }

/*
int Point3::ClosestSphere(Point3 &pt, std::vector<Sphere>& sphere_arr) {
    int idx = -1;
    for (int i = 0; i < sphere_arr.size(); i++) {
        Sphere sphere_ = sphere_arr.at(i);
        float d = (sphere_.GetOrigin() - this).Length();
        if (std::abs(sphere_.GetRadius() - d) < MATH_DIV_OFFSET) {
            idx = i;
        }
    }
    return idx;
}*/

std::ostream& operator<< (std::ostream& os, const Point3& p) {
    return os << "(" << p[0] << ", " << p[1] << ", " << p[2] << ")";
}

std::istream& operator>> (std::istream& is, Point3& p) {
    char dummy;
    return is >> dummy >> p[0] >> dummy >> p[1] >> dummy >> p[2] >> dummy;
}
