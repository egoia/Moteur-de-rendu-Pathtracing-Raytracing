#include <iostream>
#include <vector>
#include "../Vecteurs/Vector.h"
#include "Image.h"

Vector3 NO_COLLISION_COLOR = Vector3(236, 50, 255);

struct Sphere{
    Vector3 centre;
    float rayon;
    Vector3 color;
    Vector3 emited_light;

    Sphere(Vector3 c, float r, Vector3 color) : centre(c), rayon(r), color(color){}
};

struct Ray{
    Vector3 direction;
    Vector3 origin;
};

struct PointLight{
    Vector3 position;
    Vector3 color;
    float quantity;

    PointLight(Vector3 p, float q, Vector3 color) : position(p), quantity(q), color(color){}
};

struct Camera{
    Vector3 position;
    std::vector<Vector3> pixels;
    std::vector<float> depthMap;
    float distance_focale;
    float renderDistance;
    float width;
    float height;
    int width_in_pixel;
    int height_in_pixel;

    void make_plane(){
        pixels = std::vector<Vector3>(width_in_pixel * height_in_pixel, Vector3(0, 0, 0));
        float pixel_height = height / (float)height_in_pixel;
        float pixel_width = width / (float)width_in_pixel;
        Vector3 position_zero = position + Vector3(-width/2.f + pixel_width/2.f, -height/2.f + pixel_height/2.f, distance_focale);
        for(int i = 0; i<height_in_pixel; i++){
            for(int j =0; j<width_in_pixel; j++){
                pixels[i*width_in_pixel +j] = (position_zero + Vector3(j*pixel_width, i*pixel_height, 0) );
            }
        }
    };

    Ray make_ray(int x, int y){
        Ray ray;
        ray.direction = pixels[y*width_in_pixel + x] - position;
        ray.direction.normalize();
        ray.origin = position;
        return ray;
    };


    PPM depth_raytrace(std::vector<Sphere> spheres){
        make_plane();
        PPM image;
        image.maxValue = 255;
        image.height = height_in_pixel;
        image.width = width_in_pixel;
        image.pixels = std::vector<Vector3>(image.width * image.height, NO_COLLISION_COLOR);
        depthMap = std::vector<float>(image.width * image.height, std::numeric_limits<float>::infinity());
        float max = 0;
        float min = std::numeric_limits<float>::infinity();

        for(int i = 0; i<height_in_pixel; i++){
            for(int j = 0; j<width_in_pixel; j++){
                for(int k = 0; k<spheres.size(); k++){
                    Ray ray = make_ray(j,i);
                    float t;
                    if(send_depth_ray(ray, spheres[k], t)){
                        if(depthMap[i*width_in_pixel +j]>t){
                            if(t<min)min = t;
                            if(t>max)max = t;
                            depthMap[i*width_in_pixel +j] = t;
                            image.pixels[i*width_in_pixel +j] = spheres[k].color;
                        }
                    }
                }
                

            }
        }
        depth_toneMap(min, max, image);
        return image;
    };

    PPM raytrace(std::vector<Sphere> spheres, std::vector<PointLight> lights){
        make_plane();
        PPM image;
        image.maxValue = 255;
        image.height = height_in_pixel;
        image.width = width_in_pixel;
        image.pixels = std::vector<Vector3>(image.width * image.height, NO_COLLISION_COLOR);
        depthMap = std::vector<float>(image.width * image.height, std::numeric_limits<float>::infinity());
        float max = 0;
        float min = std::numeric_limits<float>::infinity();

        for(int i = 0; i<height_in_pixel; i++){
            for(int j = 0; j<width_in_pixel; j++){
                for(int k = 0; k<spheres.size(); k++){
                    Ray ray = make_ray(j,i);
                    float t;
                    Vector3 total_light;
                    if(send_ray(ray, spheres[k], lights, t, total_light)){
                        if(depthMap[i*width_in_pixel +j]>t){
                            if(t<min)min = t;
                            if(t>max)max = t;
                            depthMap[i*width_in_pixel +j] = t;
                            image.pixels[i*width_in_pixel +j] = Vector3::Clamp(total_light, Vector3(255,255,255));
                        }
                    }
                }
                

            }
        }
        //toneMap(min, max, image);
        return image;
    };

    void depth_toneMap(float min, float max, PPM& image){
        std::cout <<min << " " << max;
         for(int i = 0; i<height_in_pixel; i++){
            for(int j = 0; j<width_in_pixel; j++){
                if(depthMap[i*width_in_pixel +j]==std::numeric_limits<float>::infinity())depthMap[i*width_in_pixel +j] = 1;
                else depthMap[i*width_in_pixel +j] = 1-(depthMap[i*width_in_pixel +j] - min) /(max-min);
                image.pixels[i*width_in_pixel +j] = image.pixels[i*width_in_pixel +j]*depthMap[i*width_in_pixel +j];
            }
        }
    };

    bool send_depth_ray(Ray ray, Sphere sphere, float& t){
        Vector3 OC = sphere.centre - ray.origin;
        float c = (Vector3::DotProduct(OC,OC)) - (sphere.rayon*sphere.rayon);
        float b = -2*Vector3::DotProduct(OC, ray.direction);
        float a = (Vector3::DotProduct(ray.direction,ray.direction));//dot(a,a) == norm2(a)
        float delta = b*b - 4 * a * c;
        if(delta>=0){
            float t1 = (-b -sqrt(delta))/(2*a);
            float t2 = (-b +sqrt(delta))/(2*a);
            if(t1>0){
                t = t1;
                return true;
            }
            else{
                return false;
            }
        }
        return false;
    };

    bool send_ray(Ray ray, int sphere_index, std::vector<PointLight> lights, std::vector<Sphere> spheres, float& t, Vector3& total_light){
        Sphere sphere = spheres[sphere_index];
        Vector3 OC = sphere.centre - ray.origin;
        float c = (Vector3::DotProduct(OC,OC)) - (sphere.rayon*sphere.rayon);
        float b = -2*Vector3::DotProduct(OC, ray.direction);
        float a = (Vector3::DotProduct(ray.direction,ray.direction));//dot(a,a) == norm2(a)
        float delta = b*b - 4 * a * c;
        total_light = Vector3();
        if(delta>=0){
            float t1 = (-b -sqrt(delta))/(2*a);
            float t2 = (-b +sqrt(delta))/(2*a);
            if(t1>0){
                t = t1;
                Vector3 contact_point = ray.origin + t*ray.direction;


                Vector3 albedo = sphere.color;
                Vector3 L_e = sphere.emited_light;
                for(int i = 0; i<lights.size(); i++){
                    

                    PointLight light = lights[i];

                    //ombre portées
                    float t = (contact_point-light.position).magnitude();
                    //Ray to light
                    for(int i =0; i<spheres.size(); i++){
                        if(i!=sphere_index){

                        }
                    }
                    float light_visibility = 1;// V(P, L_p)
                    float L_emit = light.quantity;
                    float D = 1;//(contact_point-light.position).magnitude();
                    Vector3 N = (contact_point-sphere.centre).normalized();
                    Vector3 L_i = (light.position - contact_point).normalized();

                    Vector3 L_o = L_e + light_visibility * (L_emit / (D*D)) * albedo * std::max(Vector3::DotProduct(N,L_i), 0.f);
                    total_light = total_light+L_o;
                }
                return true;
            }
        }

        

        return false;
    }
};




int main(int argc, char *argv[]){
    constexpr int wallSphereRadius = 5000;

    PointLight light_1 = PointLight(Vector3(), 1, Vector3(255,255,255));
    Sphere wall_1 = Sphere(Vector3(0,0,125 + wallSphereRadius), wallSphereRadius, Vector3(180,180,180));
    Sphere wall_2 = Sphere(Vector3(wallSphereRadius + 125, 0, 0), wallSphereRadius, Vector3(180,180,180));
    Sphere wall_3 = Sphere(Vector3(-wallSphereRadius - 125,0,0), wallSphereRadius, Vector3(180,180,180));
    Sphere wall_4 = Sphere(Vector3(0,wallSphereRadius + 125,0), wallSphereRadius, Vector3(180,180,180));
    Sphere wall_5 = Sphere(Vector3(0,-wallSphereRadius - 125,0), wallSphereRadius, Vector3(180,180,180));

    
    Sphere sphere = Sphere(Vector3(0,50,100), 40, Vector3(0,0,255));
    Sphere sphere2 = Sphere(Vector3(50,0,50), 30, Vector3(0,255,0));
    Sphere sphere3 = Sphere(Vector3(-50,0,0), 30, Vector3(255,0,0));
    
    Camera cam;
    cam.position = Vector3(0, 0,-250);
    cam.width_in_pixel = 1280;
    cam.height_in_pixel = 720;
    cam.height = 10;
    cam.width = 16/9.f *10;
    cam.renderDistance = 30;
    cam.distance_focale = 10;

    std::vector<Sphere> spheres = {sphere, sphere2, sphere3, wall_1, wall_2, wall_3, wall_4, wall_5};
    std::vector<PointLight> lights = {light_1};
    PPM image = cam.raytrace(spheres, lights);
    writePPM("scene.ppm", image);
    return 0;
};