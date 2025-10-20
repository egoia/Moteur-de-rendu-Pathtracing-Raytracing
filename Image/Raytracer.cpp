#include <iostream>
#include <vector>
#include "../Vecteurs/Vector.h"
#include "Image.h"

Vector3 NO_COLLISION_COLOR = Vector3(236, 50, 255);

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


    PPM raytrace(std::vector<Sphere> spheres){
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
        toneMap(min, max, image);
        return image;
    };

    void toneMap(float min, float max, PPM& image){
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
};




int main(int argc, char *argv[]){
    Sphere sphere;
    sphere.centre = Vector3();
    sphere.rayon = 10;
    sphere.color = Vector3(0,0,255);

    Sphere sphere2;
    sphere2.centre = Vector3(5,0,-5);
    sphere2.rayon = 5;
    sphere2.color = Vector3(0,255,0);
    
    Camera cam;
    cam.position = Vector3(0, 0,-30);
    cam.width_in_pixel = 1280;
    cam.height_in_pixel = 720;
    cam.height = 10;
    cam.width = 16/9.f *10;
    cam.renderDistance = 30;
    cam.distance_focale = 10;

    std::vector<Sphere> spheres = {sphere, sphere2};

    PPM image = cam.raytrace(spheres);
    std::cerr<<"end1";
    writePPM("scene.ppm", image);
    std::cerr<<"end";
    return 0;
};