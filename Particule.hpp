#ifndef PARTICULE_HPP
#define PARTICULE_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <array>
#include <cstdlib>  

constexpr int D = 2; //dimension on peut changer en 3, 4...
using coord = std::array<double, D>;

inline coord zeros() {
    coord z{};
    z.fill(0.0);
    return z;
}

inline double norm2(const coord& a) {
    double s = 0.0;
    for (int i = 0; i < D; ++i) s += a[i] * a[i];
    return s;
}

// _____________________________________________________________________________________
// CLASSE PARTICULE
class Particule {
public:
    coord position;
    coord vitesse;
    coord force;
    double masse_p;
    Particule* particule_suivante;
    std::vector<coord> liste_position;

    // Constructeur OBLIGATOIRE: on force m
    Particule(const coord& p, const coord& v, double m, const coord& f, Particule* next = nullptr);

    // Méthodes
    void deplacer(const coord& new_pos);
    void ajouter_position(const coord& pos);
    void saute_mouton(double delta_t);
    void print() const;
    void print_liste() const;

    // Getters 
    double getMasse() const;
    const coord& getPosition() const;
    const coord& getVitesse() const;
    const coord& getForce() const;
    Particule* getSuivante();
    const Particule* getSuivante() const;
};

// Fonctions globales
coord forceEntreParticules(const Particule* p1, const Particule* p2);
coord forceTotaleSurParticule(Particule* particule);
Particule* creerSysteme(int N);

#endif // PARTICULE_HPP
