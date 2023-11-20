#include "Vector3.h"
#include "Point3.h"

#define MATH_DIV_OFFSET 1e-8

Vector3::Vector3() {
    v[0] = 0.0;
    v[1] = 0.0;
    v[2] = 0.0;
}

Vector3::Vector3(float x, float y, float z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}

Vector3::Vector3(float* ptr) {
    v[0] = ptr[0];
    v[1] = ptr[1];
    v[2] = ptr[2];
}

Vector3::Vector3(const Vector3& other) {
    v[0] = other[0];
    v[1] = other[1];
    v[2] = other[2];
}

Vector3::~Vector3() {
}

bool Vector3::operator==(const Vector3& other) const {
    return (fabs(other[0] - v[0]) < MATH_DIV_OFFSET &&
        fabs(other[1] - v[1]) < MATH_DIV_OFFSET &&
        fabs(other[2] - v[2]) < MATH_DIV_OFFSET);
}

bool Vector3::operator!=(const Vector3& other) const {
    return (fabs(other[0] - v[0]) >= MATH_DIV_OFFSET || 
        fabs(other[1] - v[1]) >= MATH_DIV_OFFSET || 
        fabs(other[2] - v[2]) >= MATH_DIV_OFFSET);
}

Vector3& Vector3::operator=(const Vector3& other) {
    v[0] = other[0];
    v[1] = other[1];
    v[2] = other[2];
    return *this;
}

float Vector3::operator[](const int i) const {
    if ((i >= 0) && (i <= 2)) {
        return v[i];
    }
    else {
        // w component of a vector is 0 so return the constant 0.0
        return 0.0;
    }
}

float& Vector3::operator[](const int i) {
    return v[i];
}

float Vector3::Dot(const Vector3& other) const {
    return v[0] * other[0] + v[1] * other[1] + v[2] * other[2];
}

Vector3 Vector3::Cross(const Vector3& other) const {
    return Vector3(v[1] * other[2] - v[2] * other[1],
        v[2] * other[0] - v[0] * other[2],
        v[0] * other[1] - v[1] * other[0]);
}

float Vector3::Length() const {
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void Vector3::Normalize() {
    float sizeSq =  v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    if (sizeSq < MATH_DIV_OFFSET) {
        return; // do nothing to zero vectors;
    }
    float scaleFactor = (float)1.0 / (float)sqrt(sizeSq);
    v[0] *= scaleFactor;
    v[1] *= scaleFactor;
    v[2] *= scaleFactor;
}

Vector3 Vector3::ToUnit() const {
    Vector3 v(*this);
    v.Normalize();
    return v;
}

static const Vector3 s_zerov3d = Vector3(0, 0, 0);

static const Vector3 s_onev3d = Vector3(1, 1, 1);

static const Vector3 s_unitxv3d = Vector3(1, 0, 0);

static const Vector3 s_unityv3d = Vector3(0, 1, 0);

static const Vector3 s_unitzv3d = Vector3(0, 0, 1);


const Vector3& Vector3::ZeroVector() { return s_zerov3d; }

const Vector3& Vector3::OneVector() { return s_onev3d; }

const Vector3& Vector3::UnitVectorX() { return s_unitxv3d; }

const Vector3& Vector3::UnitVectorY() { return s_unityv3d; }

const Vector3& Vector3::UnitVectorZ() { return s_unitzv3d; }

Vector3 Vector3::Normalize(const Vector3& v) {
    return v.ToUnit();
}

Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2) {
    return v1.Cross(v2);
}

float Vector3::Dot(const Vector3& v1, const Vector3& v2) {
    return v1.Dot(v2);
}

Vector3 operator/(const Vector3& v, const float s) {
    const float invS = 1 / s;
    return Vector3(v[0] * invS, v[1] * invS, v[2] * invS);
}

Vector3 operator*(const float s, const Vector3& v) {
    return Vector3(v[0] * s, v[1] * s, v[2] * s);
}

Vector3 operator*(const Vector3& v, const float s) {
    return Vector3(v[0] * s, v[1] * s, v[2] * s);
}

Vector3 operator-(const Vector3& v) {
    return Vector3(-v[0], -v[1], -v[2]);
}

Point3 operator+(const Vector3& v, const Point3& p) {
    return Point3(p[0] + v[0], p[1] + v[1], p[2] + v[2]);
};

Point3 operator+(const Point3& p, const Vector3& v) {
    return Point3(p[0] + v[0], p[1] + v[1], p[2] + v[2]);
}

Vector3 operator+(const Vector3& v1, const Vector3& v2) {
    return Vector3(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]);
}

Point3 operator-(const Point3& p, const Vector3& v) {
    return Point3(p[0] - v[0], p[1] - v[1], p[2] - v[2]);
}

Vector3 operator-(const Vector3& v1, const Vector3& v2) {
    return Vector3(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]);
}

Vector3 operator-(const Point3& p1, const Point3& p2) {
    return Vector3(p1[0] - p2[0], p1[1] - p2[1], p1[2] - p2[2]);
}


std::ostream& operator<< (std::ostream& os, const Vector3& v) {
    return os << "<" << v[0] << ", " << v[1] << ", " << v[2] << ">";
}

std::istream& operator>> (std::istream& is, Vector3& v) {
    char dummy;
    return is >> dummy >> v[0] >> dummy >> v[1] >> dummy >> v[2] >> dummy;
}
