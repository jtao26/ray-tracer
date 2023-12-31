#include "Color.h"
#include <math.h>

Color::Color() {
    c[0] = 0.0;
    c[1] = 0.0;
    c[2] = 0.0;
}

Color::Color(float red, float green, float blue) {
    c[0] = red;
    c[1] = green;
    c[2] = blue;
}

Color::Color(float* ptr) {
    c[0] = ptr[0];
    c[1] = ptr[1];
    c[2] = ptr[2];
}

Color::Color(const Color& other) {
    c[0] = other[0];
    c[1] = other[1];
    c[2] = other[2];
}

Color::Color(const std::vector<float>& vals) {
    c[0] = vals[0];
    c[1] = vals[1];
    c[2] = vals[2];
}


Color::~Color() {
}

Color Color::Time(float other) const {
    return Color(c[0] * other, c[1] * other, c[2] * other);
}

bool Color::operator==(const Color& other) const {
    return ((other[0] == c[0]) &&
        (other[1] == c[1]) &&
        (other[2] == c[2]));
}

bool Color::operator!=(const Color& other) const {
    return ((other[0] != c[0]) ||
        (other[1] != c[1]) ||
        (other[2] != c[2]));
}

Color& Color::operator=(const Color& other) {
    c[0] = other[0];
    c[1] = other[1];
    c[2] = other[2];
    return *this;
}

float Color::operator[](const int i) const {
    return c[i];
}

float& Color::operator[](const int i) {
    return c[i];
}


const float* Color::value_ptr() const {
    return c;
}

std::vector<float> Color::ToVector() const {
    std::vector<float> v;
    v.push_back(c[0]);
    v.push_back(c[1]);
    v.push_back(c[2]);
    return v;
}

std::ostream& operator<< (std::ostream& os, const Color& c) {
    return os << c[0] << " " << c[1] << " " << c[2] << " ";
}

std::istream& operator>> (std::istream& is, Color& c) {
    // format:  (r, g, b, a)
    char dummy;
    return is >> dummy >> c[0] >> dummy >> c[1] >> dummy >> c[2] >> dummy;
}