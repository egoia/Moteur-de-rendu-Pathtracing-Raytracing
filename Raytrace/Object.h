#include <iostream>

#include "../Vecteurs/Vector.h"

struct Mesh{
    public :
        Vector3* vertices;
        int* triangles;
        Vector3* normales;

        static Mesh ReadOBJ(std::string path);
};

class Object3D{
    public:
         Vector3 position;

};