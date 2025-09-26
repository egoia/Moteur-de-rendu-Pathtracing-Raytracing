#include <iostream>
#include <math.h>
#include <sstream>

struct Vector3{
    float x;
    float y;
    float z;

    public : 
        
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

        std::string toString() const; 

        Vector3 operator+(const Vector3 a) const;
        Vector3 operator-(const Vector3 a) const;
        Vector3 operator-() const;
        Vector3 operator*(const float a) const;
        Vector3 operator/(const float a) const;
        bool operator==(const Vector3 a) const;

        Vector3 normalize();// ne pas trop utiliser
        Vector3 normalized() const;
        float magnitude() const;

        Vector3 rotate(const Vector3 axe, float rotation/*en degrès*/) const;

        static float DotProduct(const Vector3 a, const Vector3 b);
        static Vector3 CrossProduct(const Vector3 a, const Vector3 b);
        static float Angle(const Vector3 a, const Vector3 b);

};

Vector3 operator*(float a, const Vector3 b);
Vector3 operator/(float a, const Vector3 b);
std::ostream& operator<<(std::ostream& os, const Vector3 v);