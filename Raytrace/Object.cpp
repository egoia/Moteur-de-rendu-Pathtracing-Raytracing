#include "Object.h"

Mesh Mesh::ReadOBJ(std::string path){
    
};

bool Sphere::intersect(Ray ray, HitRay& hit){
    Vector3 OC = center - ray.origin;
    float c = (Vector3::DotProduct(OC,OC)) - (radius*radius);
    float b = -2*Vector3::DotProduct(OC, ray.direction);
    float a = ray.direction.norm2();//dot(a,a) == norm2(a)
    float delta = b*b - 4 * a * c;
    if(delta>=0){
        float t1 = (-b -sqrt(delta))/(2*a);
        float t2 = (-b +sqrt(delta))/(2*a);
        if(t1>0){
            hit.t = t1;
            hit.mat = material;
            hit.contact_point = hit.t*ray.direction + ray.origin;
            hit.normal = (center - hit.contact_point).normalized();
            return true;
        }
        else{
            return false;
        }
    }
    return false;
};

bool Ray::intersects_triangle(Vector3 A, Vector3 B, Vector3 C, HitRay& hit){
    constexpr float epsilon = std::numeric_limits<float>::epsilon();

    Vector3 edge1 = B - A;
    Vector3 edge2 = C - A;
    Vector3 ray_cross_e2 = Vector3::CrossProduct(direction, edge2);
    float det = Vector3::DotProduct(edge1, ray_cross_e2);

    if (det > -epsilon && det < epsilon)
        return {};    // This ray is parallel to this triangle.

    float inv_det = 1.0 / det;
    Vector3 s = origin - A;
    float u = inv_det * Vector3::DotProduct(s, ray_cross_e2);

    if ((u < 0 && abs(u) > epsilon) || (u > 1 && abs(u-1) > epsilon))
        return {};

    Vector3 s_cross_e1 = Vector3::CrossProduct(s, edge1);
    float v = inv_det * Vector3::DotProduct(direction, s_cross_e1);

    if ((v < 0 && abs(v) > epsilon) || (u + v > 1 && abs(u + v - 1) > epsilon))
        return {};

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = inv_det * Vector3::DotProduct(edge2, s_cross_e1);

    if (t > epsilon) // ray intersection
    {
        hit.t = t;
        hit.contact_point = origin + direction * t;
        hit.normal = Vector3::CrossProduct(edge1, edge2);
        return true; //origin + direction * t;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
};

bool Mesh::intersect(Ray ray, HitRay& hit) {
    for (size_t i = 0; i < triangles.size(); i+=3)
    {
        HitRay temp;
        if(ray.intersects_triangle(vertices[triangles[i]], vertices[triangles[i+1]], vertices[triangles[i+2]], temp)){
            temp.mat = material;
            hit = temp;
            return true;
        }
    }
    return false;
    
};