#ifndef MAIN_HPP
#define MAIN_HPP
#include <iostream>
#include <cmath>
using namespace std;
const int D = 2;


class Particle{
    public:
        double mass;
        double position[D];
        double speed[D];
        double force[D];
        double ** film;
        int film_length;
        Particle * next;
        
};


class Box{
    public:
        int niveau;
        int nb_filles;
        double center[D];
        double barycentre[D];
        double mass;
        Particle * particle;
        Box * daughter;
        Box * sister;
        Box(); //création de la première boite, la plus grande, de niveau 0 et centrée
        Box(Box * parent); //création des boites filles à partir de la boite mère
};
Box:: Box(){
    this->niveau = 0;
    for(int i = 0; i < D; i++){
        this-> center[i] = 0.0;
        this-> barycentre[i] = 0.0;
    }
    this->mass = 0.0;
    this->particle = NULL;
    this->daughter = NULL;
    this->sister = NULL;
}

Box :: Box(Box * parent){
    if (parent->nb_filles >= pow(2,D)){
        cout << "Erreur : création d'une boîte fille alors que la boîte mère est pleine" << endl;
        exit(1);
    }

}
#endif // MAIN_HPP