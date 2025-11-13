#include <iostream>

#pragma once

class Utils{
    public : 
        static float RandomFloat(){
            return (std::rand() / (RAND_MAX+1.0));
        }
};
