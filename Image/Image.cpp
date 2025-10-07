#include <iostream>
#include <fstream>
#include <vector>
#include "../Vecteurs/Vector.h"
#include "Image.h"



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
    
    std::cerr <<"lala";

    std::ostringstream buffer;

    std::string format = "P3";
    buffer << format << std::endl;

    buffer << image.width << " "<< image.height << std::endl;
    buffer << image.maxValue << std::endl;
    for(int i = 0; i<image.height; i++){
        for(int j = 0; j<image.width; j++){
            buffer << image.pixels[i*image.width + j].x << " ";
            buffer << image.pixels[i*image.width + j].y << " ";
            buffer << image.pixels[i*image.width + j].z << "\n";
        }
    }
    imageStream << buffer.str();
    
    std::cerr <<"end";
    imageStream.close();
    return 0;
};

/*
int main(int argc, char *argv[]){
    PPM image;
    image.maxValue = 255;
    image.height = 720;
    image.width = 1280;
    image.pixels = std::vector<Vector3>(image.width * image.height, Vector3(255, 0, 0));
    writePPM("rouge.ppm", image);
};*/