#include <iostream>
#include <vector>
#include "../Vecteurs/Vector.h"

struct Material{
    Vector3 emited_light;
    Vector3 color;

    Material(Vector3 color) : color(color), emited_light(Vector3()){};
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

    bool intersects_triangle(Vector3 A, Vector3 B, Vector3 C, HitRay& hit);
        
};

struct Visual3D{
    Material material;
    public:
        virtual bool intersect(Ray ray, HitRay& hit);

    Visual3D(Material mat) : material(mat){}
};

struct Mesh : Visual3D{
    public :
        Vector3 center;
        std::vector<Vector3> vertices;
        std::vector<int> triangles;
        std::vector<Vector3> normals;

        static Mesh ReadOBJ(std::string path);

        bool intersect(Ray ray, HitRay& hit) override;
};

struct Sphere : Visual3D{
    float radius;
    Vector3 center;

    Sphere(Vector3 c, float r, Material mat) : center(c), radius(r), Visual3D(mat){}

    bool intersect(Ray ray, HitRay& hit) override;
};

struct PointLight{
    Vector3 position;
    Vector3 color;
    float quantity;

    PointLight(Vector3 p, float q, Vector3 color) : position(p), quantity(q), color(color){}
};

