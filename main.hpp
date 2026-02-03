#ifndef MAIN_HPP
#define MAIN_HPP
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
    Particule(coord p = {0.,0.}, coord v = {0., 0.}, double m = 1., coord f = {0.,0.}, Particule* p_s = nullptr)
        : position(p), vitesse(v), masse_p(m), force(f), particule_suivante(p_s) {
            liste_position.push_back(position);
        }

    // Méthodes demandées
    void deplacer(const coord& new_pos);
    void ajouter_position(const coord& pos);
    void saute_mouton(double delta_t); // Implémente l'ordre 2 stable 
    void print() const;
    void print_liste() const; // afficher des infos sur la liste de particules en partant de celle-ci
};
inline void Particule::deplacer(const coord& new_pos) { position=new_pos; }

inline void Particule::ajouter_position(const coord& pos) {
    liste_position.push_back(pos);
}
inline void Particule::saute_mouton(double delta_t) {
    // Calcul de l'accélération
    double ax = force.x / masse_p;
    double ay = force.y / masse_p;

    // Intégration vitesse (demi-pas)
    vitesse.x += ax * delta_t * 0.5;
    vitesse.y += ay * delta_t * 0.5;

    // Intégration position (pas complet)
    position.x += vitesse.x * delta_t;
    position.y += vitesse.y * delta_t;

    // Intégration vitesse (demi-pas)
    vitesse.x += ax * delta_t * 0.5;
    vitesse.y += ay * delta_t * 0.5;
}

inline void Particule::print() const {
    cout << "Position: (" << position.x << ", " << position.y << "), "
         << "Vitesse: (" << vitesse.x << ", " << vitesse.y << "), "
         << "Masse: " << masse_p << endl;
}
inline void Particule::print_liste() const {
    const Particule* current = this;

    // sécurité si liste mal construite
    if (!particule_suivante) {
        current->print();
        cout << "Fin de la liste des particules." << endl;
        return;
    }

    do {
        current->print();
        current = current->particule_suivante;
    } while (current && current != this);

    cout << "Fin de la liste des particules." << endl;
}
// Force exercée par p2 sur p1 (gravitation)
inline coord forceEntreParticules(const Particule* p1, const Particule* p2) {
    coord F{0.0, 0.0};
    if (!p1 || !p2) return F;

    const double G = 1.0;     // à adapter si nécessaire
    const double eps = 1e-12; // évite division par 0

    double dx = p2->position.x - p1->position.x;
    double dy = p2->position.y - p1->position.y;

    double r2 = dx*dx + dy*dy + eps;
    double r  = std::sqrt(r2);

    // F = G*m1*m2 / r^3 * (dx,dy)
    double facteur = G * p1->masse_p * p2->masse_p / (r2 * r);

    F.x = facteur * dx;
    F.y = facteur * dy;

    return F;
}
// Force totale subie par une particule (somme des forces de toutes les autres)
inline coord forceTotaleSurParticule(Particule* particule) {
    coord Ftot{0.0, 0.0};
    if (!particule || !particule->particule_suivante) return Ftot;

    Particule* courant = particule->particule_suivante;

    while (courant && courant != particule) {
        coord F = forceEntreParticules(particule, courant);
        Ftot.x += F.x;
        Ftot.y += F.y;
        courant = courant->particule_suivante;
    }

    return Ftot;
}
inline Particule* creerSysteme(int N) {
    if (N <= 0) return nullptr;

    Particule* p0 = new Particule();
    Particule* courant = p0;

    for (int i = 1; i < N; ++i) {
        Particule* p = new Particule(
            {double(rand()) / RAND_MAX, double(rand()) / RAND_MAX},
            {0.0, 0.0},
            1.0
        );
        courant->particule_suivante = p;
        courant = p;
    }

    courant->particule_suivante = p0; // liste circulaire
    return p0;
}

/*class Box{
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
    for(int i=0; i<D; i++){
        this->center[i] = 0.0;
        this->barycentre[i] = 0.0;
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

}*/
#endif // MAIN_HPP