#include <iostream>
#include <fstream>
#include <vector>
#include "../Vecteurs/Vector.h"

struct PPM{
    public:
        int height, width;
        int maxValue;
        std::vector<Vector3> pixels;
};

int readPPM(std::string fileName, PPM& image){
    std::fstream imageStream;
    imageStream.open(fileName, std::ios::in);
    if (!imageStream.is_open()) {
        std::cerr << "Erreur d'ouverture" << std::endl;
        return 1;
    }

    std::string format;
    imageStream >> format;
    if(format!="P3"){
        std::cerr << "Format du fichier non supporté"<<std::endl;
        return 1;
    }

    imageStream >> image.width >> image.height;
    int size = image.height*image.width;
    image.pixels.resize(size);
    imageStream >> image.maxValue;
    for(int i = 0; i<image.height; i++){
        for(int j = 0; j<image.width; j++){
            imageStream >> image.pixels[i*image.width + j].x;
            imageStream >> image.pixels[i*image.width + j].y;
            imageStream >> image.pixels[i*image.width + j].z;
        } 
    }
    imageStream.close();
    return 0;
};

int writePPM(std::string fileName, PPM image){
    std::fstream imageStream;
    imageStream.open(fileName, std::ios::out);
    if (!imageStream.is_open()) {
        std::cerr << "Erreur d'ouverture" << std::endl;
        return 1;
    }

    std::string format = "P3";
    imageStream << format << std::endl;

    imageStream << image.width << " "<< image.height << std::endl;
    imageStream << image.maxValue << std::endl;
    for(int i = 0; i<image.height; i++){
        for(int j = 0; j<image.width; j++){
            imageStream << image.pixels[i*image.width + j].x << " ";
            imageStream << image.pixels[i*image.width + j].y << " ";
            imageStream << image.pixels[i*image.width + j].z << std::endl;
        }
    }
    imageStream.close();
    return 0;
};


