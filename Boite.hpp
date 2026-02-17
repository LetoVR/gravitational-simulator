#ifndef BOITE_HPP
#define BOITE_HPP

#include "Particule.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

class Boite {
private:
    int niveau;                     // Niveau dans la hiérarchie
    coord centre;     // Centre de la boîte (x, y, z)
    coord centreMasse;// Centre de masse des particules contenues
    double masse;                   // Masse cumulée
    Particule* particule;           // Pointeur vers particule si terminale 
    Boite* premiereFille;           // Pointeur vers première sous-boîte
    Boite* soeur;                   // Pointeur vers boîte soeur
    double taille;                  // Taille de la boîte
    int dimension;                  // 2D ou 3D
    
public:
    // Constructeurs
    Boite(int niv, coord cent, double t, int dim = 3);
    ~Boite();
    
    // Méthodes principales
    void ajouterParticule(Particule* p);
    void supprimerParticule(Particule* p);
    void calculerForces(Boite* autre, double theta = 0.5);
    void mettreAJourCentreMasse();
    
    // Getters
    int getNiveau() const { return niveau; }
    double getTaille() const { return taille; }
    coord getCentre() const { return centre; }
    coord getCentreMasse() const { return centreMasse; }
    double getMasse() const { return masse; }
    Boite* getPremiereFille() const { return premiereFille; }
    Boite* getSoeur() const { return soeur; }
    Particule* getParticule() const { return particule; }
    
    // Setters
    void setSoeur(Boite* s) { soeur = s; }
    // void setPremiereFille(Boite* f) { premiereFille = f; } non utilisée finalement
    
    // Méthodes utilitaires
    bool estTerminale() const;
    bool estVide() const;
    bool contientParticule(const Particule* p) const;
    double calculerDistance(const Boite* autre) const; //Calcul la distance entre les centres de masse de deux boîtes
    void diviserBoite();
    void nettoyerBoitesVides();
    

    // Pour l'affichage test
    bool contient(double x, double y) const;
    Boite* trouverBoiteTerminale(double x, double y);
private:
    // Méthodes internes
    bool doitDiviser() const;
    int getIndiceSousBoite(const coord & position) const;
    coord getCentreSousBoite(int indice) const;
};

#endif