#ifndef MAIN_HPP
#define MAIN_HPP
#include <iostream>
using namespace std;

class Particle{
    public:
        double position[3];
        double speed[3];
        double force[3];
        double ** film;
        int film_length;
        Particle * next;


};


#endif // MAIN_HPP