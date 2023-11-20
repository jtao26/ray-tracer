#ifndef VECTOR3_H_
#define VECTOR3_H_

#include <iostream>
#include <cmath>

struct Point3;

struct Vector3 {
public:

    /// Default constructor to create zero vector
    Vector3();

    /// Constructs a vector (x,y,z)
    Vector3(float x, float y, float z);

    /// Constructs a vector given a pointer to x,y,z data
    Vector3(float* v);

    /// Copy constructor for vector
    Vector3(const Vector3& v);

    /// Vector destructor
    virtual ~Vector3();

    /// Check for "equality", taking floating point imprecision into account
    bool operator==(const Vector3& v) const;

    /// Check for "inequality", taking floating point imprecision into account
    bool operator!=(const Vector3& v) const;

    /// Vector assignment operator
    Vector3& operator=(const Vector3& v);

    /// Read only access to the ith coordinate of the vector.
    float operator[](const int i) const;

    /// Returns a reference to the ith coordinate of the vector.  Use this
    /// accessor if you wish to set the coordinate rather than request value.
    /// Example:
    /// ~~~
    /// Vector3 a;
    /// a[0] = 5.0; // set the x-coordinate of the vector
    /// ~~~
    float& operator[](const int i);

    /// Read only access to the x coordinate.  Can also use my_vector[0].  Use
    /// the my_vector[0] = 1.0; form if you need to set the value.
    float x() const { return v[0]; }

    /// Read only access to the y coordinate.  Can also use my_vector[1].  Use
    /// the my_vector[1] = 1.0; form if you need to set the value.
    float y() const { return v[1]; }

    /// Read only access to the z coordinate.  Can also use my_vector[2].  Use
    /// the my_vector[2] = 1.0; form if you need to set the value.
    float z() const { return v[2]; }


    // --- Vector operations ---

    /** Returns "this dot v", for example:
     ~~~
     Vector3 a(1,0,0);
     Vector3 b(0.5,0,0);
     float c = a.Dot(b);
     ~~~
     */
    float Dot(const Vector3& v) const;

    /** Returns "this cross v", for example:
    ~~~
    Vector3 x(1,0,0);
    Vector3 y(0,1,0);
    Vector3 z = x.Cross(y);
    ~~~
    */
    Vector3 Cross(const Vector3& v) const;

    /// Returns the length of the vector
    float Length() const;

    /// Normalizes the vector by making it unit length.
    void Normalize();

    /// Returns a normalized (i.e., unit length) version of the vector without
    /// modifying the original 'this' vector.
    Vector3 ToUnit() const;

    /// (0,0,0) - a shortcut for Vector (0, 0, 0)
    const Vector3& ZeroVector();

    /// (1,1,1) - a shortcut for Vector (1, 1, 1)
    const Vector3& OneVector();

    /// (1,0,0) - a shortcut for Vector (1, 0, 0)
    const Vector3& UnitVectorX();

    /// (0,1,0) - a shortcut for Vector (0, 1, 0)
    const  Vector3& UnitVectorY();

    /// (0,0,1) - a shortcut for Vector (0, 0, 1)
    const Vector3& UnitVectorZ();


    /** Returns a new vector that is the unit version of v.  This is just an
     alternative syntax for ToUnit().  Example:
     ~~~
     Vector3 a(100,150,80);
     Vector3 b = Vector3::Normalize(a);
     Vector3 c = a.ToUnit();
     // b and c are the same.
     ~~~
     */
    static Vector3 Normalize(const Vector3& v);

    /** Returns v1 cross v2.  This is just an alternative syntax for Cross().
     Example:
     ~~~
     Vector3 x(1,0,0);
     Vector3 y(0,1,0);
     Vector3 z1 = Vector3::Cross(x,y);
     Vector3 z2 = x.Cross(y);
     // z1 and z2 are the same.
     ~~~
     */
    static Vector3 Cross(const Vector3& v1, const Vector3& v2);

    /** Returns v1 dot v2.  This is just an alternative syntax for Dot().
     Example:
     ~~~
     Vector3 a(1,0,0);
     Vector3 b(0.5,0,0);
     Vector3 c1 = a.Dot(b);
     Vector3 c2 = Vector3::Dot(a,b);
     // c1 and c2 are the same.
     ~~~
     */
    static float Dot(const Vector3& v1, const Vector3& v2);

private:
    float v[3];
};

/// Divide the vector by the scalar s
Vector3 operator/(const Vector3& v, const float s);

/// Multiply the vector by the scalar s
Vector3 operator*(const float s, const Vector3& v);

/// Multiply the vector by the scalar s
Vector3 operator*(const Vector3& v, const float s);

/// Negate the vector
Vector3 operator-(const Vector3& v);

/// Adds a vector and a point, returns a point
Point3 operator+(const Vector3& v, const Point3& p);

/// Adds a point and a vector, returns a point
Point3 operator+(const Point3& p, const Vector3& v);

/// Adds a vector and a vector, returns a vector
Vector3 operator+(const Vector3& v1, const Vector3& v2);

// Note: no (point + point) operator, that's an undefined operation

/// Subtracts a vector from a point, returns a point
Point3 operator-(const Point3& p, const Vector3& v);

/// Subtracts v2 from v1, returns a vector
Vector3 operator-(const Vector3& v1, const Vector3& v2);

/// Returns the vector spanning p1 and p2
Vector3 operator-(const Point3& p1, const Point3& p2);

std::ostream& operator<< (std::ostream& os, const Vector3& v);
std::istream& operator>> (std::istream& is, Vector3& v);

#endif // !VECTOR3_H_
