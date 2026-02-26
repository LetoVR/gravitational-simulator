#ifndef PARTICULE_HPP
#define PARTICULE_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <array>
#include <cstdlib>  
#include <random>

constexpr int D = 3; //dimension on peut changer en 3, 4...
using coord = std::array<double, D>; // typedef std::array<double, D> coord pour représenter les coordonnées (position, vitesse, force)
const double G = 1.0;
const double eps = 1; // facteur d'adoucissement pour éviter les singularités
// Remarque : au plus epsilon est petit au moins la simulation est stable
const double THETA = 0.5;//Le critère MAC pour déterminer le type d'interaction (exacte ou approx)


// Explique ces structure Jude stp - Leto :
// La structure coord est une structure qui contient un tableau de D doubles, représentant les coordonnées d'un point dans un espace de dimension D. 
//Par exemple, si D=2, coord contiendra les coordonnées x et y d'un point. Si D=3, coord contiendra les coordonnées x, y et z d'un point. 
//Cette structure est utilisée pour représenter la position, la vitesse et la force des particules dans notre simulation.
//zeros() est une fonction qui retourne un objet de type coord initialisé à zéro. Elle crée un objet coord, remplit tous ses éléments avec la valeur 0.0 et le retourne.
inline coord zeros() {
    coord z{};
    z.fill(0.0);
    return z;
}
// norm2() est une fonction qui calcule le carré de la norme d'un vecteur de type coord. Elle prend en entrée un objet de type coord, calcule la somme des carrés de ses éléments et retourne cette somme.
inline double norm2(const coord& a) {
    double s = 0.0;
    for (int i = 0; i < D; ++i) s += a[i] * a[i];
    return s;
}

// _____________________________________________________________________________________
// CLASSE PARTICULE
class Particule {
public:
    coord position;// position de la particule
    coord vitesse;// vitesse de la particule
    coord force;   // force exercée sur la particule
    double masse_p; // masse de la particule
    Particule* particule_suivante; // pointeur vers la particule suivante dans la liste chaînée de particules
    std::vector<coord> liste_position; // liste des positions de la particule au cours du temps, utilisée pour l'exportation des données

    // Constructeur OBLIGATOIRE: on force m
    Particule(const coord& p, const coord& v, double m, const coord& f, Particule* next = nullptr);

    // Méthodes
    void deplacer(const coord& new_pos); // Met à jour la position de la particule
    void ajouter_position(const coord& pos); // Ajoute une position à la liste des positions de la particule (pour l'exportation des données)
    void saute_mouton(double delta_t);// Met à jour la position et la vitesse de la particule en utilisant la méthode du saut de mouton 
    void print() const;// Affiche les informations de la particule (position, vitesse, masse)
    void print_liste() const;   // Affiche les informations de toutes les particules de la liste chaînée à partir de la particule courante

    // Getters 
    double getMasse() const;// Retourne la masse de la particule
    const coord& getPosition() const;// Retourne la position de la particule
    const coord& getVitesse() const;   // Retourne la vitesse de la particule
    const coord& getForce() const;//Retourne la force exercée sur la particule
    Particule* getSuivante();// Retourne un pointeur vers la particule suivante dans la liste chaînée
    const Particule* getSuivante() const;// Retourne un pointeur constant vers la particule suivante dans la liste chaînée
};

// Fonctions globales
coord forceEntreParticules(const Particule* p1, const Particule* p2);// Calcule la force gravitationnelle exercée entre deux particules
coord forceTotaleSurParticule(Particule* particule);// Calcule la force totale exercée sur une particule par toutes les autres particules de la liste chaînée
Particule* creerSysteme(int N);// Crée un système de N particules avec des positions aléatoires, des vitesses initiales à zéro et des masses égales à 1.0
void detruireSysteme(Particule* systeme);// Cette fonction libère la mémoire allouée pour les particules du système. 


#endif // PARTICULE_HPP
