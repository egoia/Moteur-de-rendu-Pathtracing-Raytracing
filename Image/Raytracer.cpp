#include <iostream>
#include <vector>
#include "../Vecteurs/Vector.h"
#include "Image.h"

struct Sphere{
    Vector3 centre;
    float rayon;
    Vector3 color;
};

struct Ray{
    Vector3 direction;
    Vector3 origin;
};

struct Camera{
    Vector3 position;
    std::vector<Vector3> pixels;
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


    PPM raytrace(Sphere sphere){
        make_plane();
        PPM image;
        image.maxValue = 255;
        image.height = height_in_pixel;
        image.width = width_in_pixel;
        image.pixels = std::vector<Vector3>(image.width * image.height, Vector3(0, 0, 0));

        for(int i = 0; i<height_in_pixel; i++){
            for(int j =0; j<width_in_pixel; j++){
                Ray ray = make_ray(j,i);
                
                float t;
                if(send_depth_ray(ray, sphere, t)){
                    image.pixels[i*width_in_pixel +j] = (renderDistance - t)/renderDistance*sphere.color;
                }
                else{
                    image.pixels[i*width_in_pixel +j] = Vector3();
                }

            }
        }
        return image;
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
};




int main(int argc, char *argv[]){
    Sphere sphere;
    sphere.centre = Vector3();
    sphere.rayon = 10;
    sphere.color = Vector3(0,0,255);
    
    Camera cam;
    cam.position = Vector3(0, 0,-30);
    cam.width_in_pixel = 1280;
    cam.height_in_pixel = 720;
    cam.height = 10;
    cam.width = 16/9.f *10;
    cam.renderDistance = 2000;
    cam.distance_focale = 10;

    PPM image = cam.raytrace(sphere);
    std::cerr<<"end1";
    writePPM("scene.ppm", image);
    std::cerr<<"end";
    return 0;
};