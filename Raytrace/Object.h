#include <iostream>
#include <vector>
#include "../Vecteurs/Vector.h"

struct Material{
    Vector3 emited_light;
    Vector3 color;

    Material(Vector3 color) :  emited_light(Vector3()), color(color){};
    Material(Vector3 color, Vector3 emited_light) : emited_light(emited_light), color(color){};
};

struct HitRay{
    public : 
        float t;
        Vector3 contact_point;
        Vector3 normal;
        Material mat;
    HitRay() : t(0), contact_point(Vector3()), normal(Vector3()), mat(Material(Vector3())) {};
};

struct Ray{
    Vector3 direction;
    Vector3 origin;

    bool intersects_triangle(Vector3 A, Vector3 B, Vector3 C, Vector3 normal_A, Vector3 normal_B, Vector3 normal_C, HitRay& hit);
        
};

struct Visual3D{
    Material material;
    public:
        virtual bool intersect(Ray ray, HitRay& hit) = 0;
        virtual ~Visual3D() = default;

    Visual3D(Material mat) : material(mat){}
    Visual3D() : material(Material(Vector3())){}
};

struct Mesh : public Visual3D{
    public :
        Vector3 center;
        std::vector<Vector3> vertices;
        std::vector<int> triangles;
        std::vector<Vector3> normals;

        static Mesh ReadOFF(std::string path);

        bool intersect(Ray ray, HitRay& hit) override;
};

struct Sphere : public Visual3D{
    float radius;
    Vector3 center;


    Sphere(Vector3 c, float r, Material mat) :  Visual3D(mat), radius(r), center(c){}

    bool intersect(Ray ray, HitRay& hit) override;
};

struct PointLight{
    Vector3 position;
    Vector3 color;
    float quantity;

    PointLight(Vector3 p, float q, Vector3 color) : position(p),  color(color), quantity(q){}
};

