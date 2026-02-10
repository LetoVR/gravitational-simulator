#ifndef PARTICULE_HPP
#define PARTICULE_HPP
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;
const int D = 2;// Dimension de la simulation (2D)
struct coord {
    double x, y; // Coordonnées dans le plan
};
// _____________________________________________________________________________________
// CLASSE PARTICULE
class Particule {
public:
    coord position;       // Position instantanée 
    coord vitesse;        // Vitesse instantanée
    coord force;          // Force gravitationnelle subie 
    double masse_p;       // Masse de la particule 
    
    std::vector<coord> liste_position; // Historique pour le film Matlab 
    Particule* particule_suivante;     // Pour la liste chaînée 

    // Constructeur avec valeurs par défaut
    Particule(coord p = {0.,0.}, coord v = {0., 0.}, double m = 1., coord f = {0.,0.}, Particule* p_s = nullptr);

    // Méthodes demandées
    void deplacer(const coord& new_pos);
    void ajouter_position(const coord& pos);
    void saute_mouton(double delta_t); // Implémente l'ordre 2 stable 
    void print() const;
    void print_liste() const; // afficher des infos sur la liste de particules en partant de celle-ci
};

// Force exercée par p2 sur p1 (gravitation)
coord forceEntreParticules(const Particule* p1, const Particule* p2);
// Force totale subie par une particule (somme des forces de toutes les autres)
coord forceTotaleSurParticule(Particule* particule);
Particule* creerSysteme(int N) ;
#endif // PARTICULE_HPP