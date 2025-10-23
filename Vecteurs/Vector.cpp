#include "Vector.h"

std::string Vector3::toString() const{
    std::ostringstream oss;
    oss << "[" << x << " ,"  << y << " ,"  << z << " ,]";
    return oss.str();
};

Vector3 Vector3::operator+(const Vector3 a)const{
    return Vector3(this->x + a.x, this->y + a.y, this->z + a.z );
}

Vector3 Vector3::operator-(const Vector3 a)const{
    return Vector3(this->x - a.x, this->y - a.y, this->z - a.z );
}

Vector3 Vector3::operator-()const{
    return Vector3(-this->x , -this->y, -this->z );
}

Vector3 Vector3::operator*(const float a)const{
    return Vector3(this->x * a, this->y * a, this->z * a );
}

Vector3 operator*(const float a, const Vector3 b){
    return b*a;
};

Vector3 Vector3::operator/(const float a)const{
    return Vector3(this->x / a, this->y / a, this->z / a );
};

Vector3 operator/(const float a, const Vector3 b){
    return b/a;
};

bool Vector3::operator==(const Vector3 a) const{
    return this->x == a.x && this->y == a.y && this->z == a.z; 
};

std::ostream& operator<<(std::ostream& os, const Vector3 v){
    return os << v.toString();
};

Vector3 Vector3::normalize(){
    *this = *this/this->magnitude();
    return *this;
}

Vector3 Vector3::normalized() const{
    if(this->magnitude()==0) return *this;
    return *this/this->magnitude();
};

float Vector3::magnitude() const{
    return std::sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
}

Vector3 Vector3::rotate(const Vector3 axe, float rotation/*en degrès*/) const{
    Vector3 normAxe = axe.normalized();
    return *this * cos(rotation) + CrossProduct(normAxe, *this) * sin(rotation)  + normAxe * DotProduct(normAxe, *this) * (1-cos(rotation));
}

float Vector3::DotProduct(const Vector3 a, const Vector3 b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
};

Vector3 Vector3::CrossProduct(const Vector3 a, const Vector3 b){
    return Vector3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
};

float Vector3::Angle(const Vector3 a, const Vector3 b){
    return acos(DotProduct(a,b)/a.magnitude()*b.magnitude());
};

Vector3 Vector3::copy(){
    return Vector3(this->x, this->y, this->z);
};

Vector3 Vector3::Clamp(const Vector3 a, const Vector3 b){
    float x = a.x > b.x ? b.x : a.x;
    float y = a.y > b.y ? b.y : a.y;
    float z = a.z > b.z ? b.z : a.z;
    return Vector3(x,y,z);
};

Vector3 Vector3::Mulitplication(const Vector3 a, const Vector3 b){
    float x = a.x * b.x ;
    float y = a.y * b.y ;
    float z = a.z * b.z ;
    return Vector3(x,y,z);
};