#ifndef POINT3_H_
#define POINT3_H_

#include <iostream>
#include <vector>
// #include "Sphere.h"

#define MATH_DIV_OFFSET 1e-8


struct Point3 {
public:
    /// Default point at the origin
    Point3();

    /// Constructs a point given (x,y,z).
    Point3(float x, float y, float z);

    /// Constructs a point given a pointer to x,y,z data
    Point3(float* p);

    /// Copy constructor for point
    Point3(const Point3& p);

    /// Point destructor
    virtual ~Point3();

    /// Check for "equality", taking floating point imprecision into account
    bool operator==(const Point3& p) const;

    /// Check for "inequality", taking floating point imprecision into account
    bool operator!=(const Point3& p) const;

    /// Assignment operator
    Point3& operator=(const Point3& p);

    /// Read only access to the ith coordinate of the point.
    float operator[](const int i) const;

    /// Returns a reference to the ith coordinate of the point.  Use this
    /// accessor if to set the coordinate rather than just request
    /// its value.
    float& operator[](const int i);

    /// Read only access to the x coordinate.  Can also use my_point[0].  Use
    /// the my_point[0] = 1.0; form if you need to set the value.
    float x() const { return p[0]; }

    /// Read only access to the y coordinate.  Can also use my_point[1].  Use
    /// the my_point[1] = 1.0; form if you need to set the value.
    float y() const { return p[1]; }

    /// Read only access to the z coordinate.  Can also use my_point[2].  Use
    /// the my_point[2] = 1.0; form if you need to set the value.
    float z() const { return p[2]; }

    /// (0,0,0) - a shortcut for Point (0, 0, 0)
    const Point3& Origin();

    /// (0,0,0) - a shortcut for Point (0, 0, 0)
    const Point3& Zero();

    /// (1,1,1) - a shortcut for Point (1, 1, 1)
    const  Point3& One();

    

private:
    float p[3];
};

std::ostream& operator<< (std::ostream& os, const Point3& p);
std::istream& operator>> (std::istream& is, Point3& p);

#endif // !POINT3_H_
