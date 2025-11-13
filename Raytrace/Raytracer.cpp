#include <iostream>
#include <vector>
#include "../Vecteurs/Vector.h"
#include "../Image/Image.h"
#include "../Utility/Utils.cpp"


Vector3 NO_COLLISION_COLOR = Vector3(236, 50, 255)/255;
int ANTIALIASING_SAMPLES = 16;
float ATTENUATION_FACTOR = 0.5;
int DIFFUSE_ITERATIONS = 1;



struct Sphere{
    Vector3 centre;
    float rayon;
    Vector3 color;
    Vector3 emited_light;

    Sphere(Vector3 c, float r, Vector3 color) : centre(c), rayon(r), color(color){}
};

struct PointLight{
    Vector3 position;
    Vector3 color;
    float quantity;

    PointLight(Vector3 p, float q, Vector3 color) : position(p), quantity(q), color(color){}
};

struct Ray{
    Vector3 direction;
    Vector3 origin;

    bool send_depth_ray( Sphere sphere, float& t){
        Vector3 OC = sphere.centre - origin;
        float c = (Vector3::DotProduct(OC,OC)) - (sphere.rayon*sphere.rayon);
        float b = -2*Vector3::DotProduct(OC, direction);
        float a = direction.norm2();//dot(a,a) == norm2(a)
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

     bool send_ray(int sphere_index, std::vector<PointLight> lights, std::vector<Sphere> spheres, float& t, Vector3& total_light, Vector3& contact_point, Vector3& normal){
        Sphere sphere = spheres[sphere_index];
        if(send_depth_ray( sphere, t)){
            contact_point = origin + t*direction;


            Vector3 albedo = sphere.color;
            Vector3 L_e = sphere.emited_light;
            Vector3 N = (contact_point-sphere.centre).normalized();
            normal = N;
            for(int i = 0; i<lights.size(); i++){

                PointLight light = lights[i];

                //ombre portées
                float t_light = (contact_point-light.position).magnitude();
                float light_visibility = 1.f;
                for(int i = 0; i<spheres.size(); i++){
                    if(i!=sphere_index){
                        Ray secondRay;
                        secondRay.direction = contact_point-light.position;
                        secondRay.direction.normalize();
                        secondRay.origin = light.position;
                        float t_temp;
                        if(secondRay.send_depth_ray(spheres[i], t_temp)){
                            if(t_temp<t_light) light_visibility = 0.f;
                        }
                    }
                }
                float L_emit = light.quantity;
                float D = 1;//(contact_point-light.position).magnitude();
                Vector3 L_i = (light.position - contact_point).normalized();

                Vector3 L_o = L_e + light_visibility * (L_emit / (D*D)) * albedo * std::max(Vector3::DotProduct(N,L_i), 0.f);
                Vector3 colored_result = Vector3::Mulitplication(L_o, light.color);
                total_light = total_light+colored_result;
            }
            //if(send_ray(diffused, ))
            return true;
        }
        return false;
    }
};


struct World{
    std::vector<Sphere> objects;
    std::vector<PointLight> lights;

    World(std::vector<Sphere> objects, std::vector<PointLight> lights) : lights(lights), objects(objects){}

    Vector3 color(Ray ray, int iterations){
        float t_min = std::numeric_limits<float>::infinity();
        Vector3 color_min = NO_COLLISION_COLOR;
        Vector3 closest_contact_point;
        Vector3 closest_normal;
        bool contact = false;
        for(int k = 0; k<objects.size(); k++){
            float t;
            Vector3 total_light;
            Vector3 contact_point;
            Vector3 normal;
            if(ray.send_ray( k, lights, objects, t, total_light, contact_point, normal)){
                if(t_min>t){
                    //if(t<min)min = t;
                    //if(t>max)max = t;
                    t_min = t;
                    color_min = Vector3::Clamp(total_light, Vector3(255,255,255)/255);
                    closest_contact_point = contact_point;
                    closest_normal = normal;
                }
                contact = true;
            }
        }
        if(contact && iterations>0){
            iterations--;
            //diffuse
            Ray diffused;
            diffused.direction = Vector3::RandomUnitOnHemisphere(closest_normal);
            diffused.direction.normalize();
            diffused.origin = closest_contact_point+closest_normal*0.001;
            return color_min+ATTENUATION_FACTOR*color(diffused, iterations);
        }
        return color_min;
    }
    
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

    Ray* make_rays(int x,int y, int nb_rays){
        Ray* rays = new Ray[nb_rays];
        float pixel_height = height / (float)height_in_pixel;
        float pixel_width = width / (float)width_in_pixel;
        for (int i = 0; i<nb_rays; i++){
            float rx = -pixel_width + (pixel_width)*Utils::RandomFloat();
            float ry = -pixel_height + (pixel_height)*Utils::RandomFloat();
            rays[i].direction = pixels[y*width_in_pixel + x]+Vector3(rx,ry,0) - position;
            rays[i].direction.normalize();
            rays[i].origin = position;
        }
        return rays;
    }


    /*PPM depth_raytrace(std::vector<Sphere> spheres){
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
    };*/

    PPM raytrace(World world){
        make_plane();
        PPM image;
        image.maxValue = 255;
        image.height = height_in_pixel;
        image.width = width_in_pixel;
        image.pixels = std::vector<Vector3>(image.width * image.height, NO_COLLISION_COLOR);
        depthMap = std::vector<float>(image.width * image.height, std::numeric_limits<float>::infinity());
        //float max = 0;
        //float min = std::numeric_limits<float>::infinity();

        #pragma omp parallel for schedule(dynamic) default(shared) firstprivate(width_in_pixel, height_in_pixel)
        for(int i = 0; i<height_in_pixel; i++){
            #pragma omp parallel for schedule(dynamic) default(shared) firstprivate(width_in_pixel, height_in_pixel)
            for(int j = 0; j<width_in_pixel; j++){
                Ray* rays = make_rays(j,i, ANTIALIASING_SAMPLES);
                Vector3 sum_color = Vector3();
                for(int l = 0; l<ANTIALIASING_SAMPLES; l++){
                    Vector3 color_min = world.color(rays[l], DIFFUSE_ITERATIONS);
                    sum_color = sum_color +color_min;
                }
                image.pixels[i*width_in_pixel +j] = sum_color/(float)ANTIALIASING_SAMPLES;
            }
        }
        //toneMap(min, max, image);
        return image;
    };

    void toPPM_format(PPM& image){
        for(int i = 0; i<height_in_pixel; i++){
            for(int j = 0; j<width_in_pixel; j++){
                image.pixels[i*width_in_pixel +j] = image.pixels[i*width_in_pixel +j]*255;
            }
        }
    }

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


};





int main(int argc, char *argv[]){
    constexpr int wallSphereRadius = 5000;

    PointLight light_1 = PointLight(Vector3(), 0.2, Vector3(255,255,255)/255);
    PointLight light_2 = PointLight(Vector3(0, 0,-250), 0.3, Vector3(150,0,200)/255);
    PointLight light_3 = PointLight(Vector3(-120, -120, 120), 0.5, Vector3(10,190,10)/255);

    Sphere wall_1 = Sphere(Vector3(0,0,125 + wallSphereRadius), wallSphereRadius, Vector3(180,250,180)/255);//fond
    Sphere wall_2 = Sphere(Vector3(wallSphereRadius + 125, 0, 0), wallSphereRadius, Vector3(180,180,0)/255);
    Sphere wall_3 = Sphere(Vector3(-wallSphereRadius - 125,0,0), wallSphereRadius, Vector3(147,23,180)/255);
    Sphere wall_4 = Sphere(Vector3(0,wallSphereRadius + 125,0), wallSphereRadius, Vector3(0,180,180)/255);
    Sphere wall_5 = Sphere(Vector3(0,-wallSphereRadius - 125,0), wallSphereRadius, Vector3(45,200,123)/255);

    
    Sphere sphere = Sphere(Vector3(0,50,100), 40, Vector3(0,0,255)/255);
    Sphere sphere2 = Sphere(Vector3(50,0,50), 30, Vector3(0,255,0)/255);
    Sphere sphere3 = Sphere(Vector3(-100,0,12), 30, Vector3(255,0,0)/255);
    
    Camera cam;
    cam.position = Vector3(0, 0,-250);
    cam.width_in_pixel = 1280;
    cam.height_in_pixel = 720;
    cam.height = 10;
    cam.width = 16/9.f *10;
    cam.renderDistance = 30;
    cam.distance_focale = 10;

    std::vector<Sphere> spheres = {sphere, sphere2, sphere3, wall_1, wall_2, wall_3, wall_4, wall_5};
    std::vector<PointLight> lights = { light_1, light_2, light_3};
    World world = World(spheres, lights);
    PPM image = cam.raytrace(world);
    cam.toPPM_format(image);
    writePPM("scene.ppm", image);
    return 0;
};