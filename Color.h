#ifndef COLOR_H_
#define COLOR_H_

#include <iostream>
#include <vector>

struct Color {
public:
    Color();

    Color(float red, float green, float blue);

    Color(float* p);

    Color(const std::vector<float>& vals);

    Color(const Color& p);

    virtual ~Color();

    bool operator==(const Color& p) const;

    bool operator!=(const Color& p) const;

    Color& operator=(const Color& p);

    float operator[](const int i) const;

    float& operator[](const int i);

    Color Time(float other) const;

    const float* value_ptr() const;

    std::vector<float> ToVector() const;

    float red() const { return c[0]; }

    float green() const { return c[1]; }
    
    float blue() const { return c[2]; }

private:
    float c[3];
};

std::ostream& operator<< (std::ostream& os, const Color& c);
std::istream& operator>> (std::istream& is, Color& c);

#endif
