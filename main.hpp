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


class Box{
    public:
        int niveau;
        double center[3];
        double barycentre[3];
        double mass;
        Particle * particle;
        Box * daughter;
        Box * sister;
};

#endif // MAIN_HPP