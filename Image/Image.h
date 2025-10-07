#include <iostream>
#include <fstream>
#include <vector>

#include "../Vecteurs/Vector.h"

#pragma once

struct PPM{
    public:
        int height, width;
        int maxValue;
        std::vector<Vector3> pixels;
};

int readPPM(std::string fileName, PPM& image);
int writePPM(std::string fileName, PPM image);