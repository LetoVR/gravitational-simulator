#ifndef BOITE_HPP
#define BOITE_HPP

#include "Particule.hpp"
#include <vector>


class Boite {
private:
    int niveau;                     // Niveau dans la hiérarchie
    std::vector<double> centre;     // Centre de la boîte (x, y, z)
    std::vector<double> centreMasse;// Centre de masse des particules contenues
    double masse;                   // Masse cumulée
    Particule* particule;           // Pointeur vers particule si terminale 
    Boite* premiereFille;           // Pointeur vers première sous-boîte
    Boite* soeur;                   // Pointeur vers boîte soeur
    double taille;                  // Taille de la boîte
    int dimension;                  // 2D ou 3D
    
public:
    // Constructeurs
    Boite(int niv, std::vector<double> cent, double t, int dim = 3);
    ~Boite();
    
    // Méthodes principales
    void ajouterParticule(Particule* p);
    void supprimerParticule(Particule* p);
    void calculerForces(Boite* autre, double theta = 0.5);
    void mettreAJourCentreMasse();
    
    // Getters
    int getNiveau() const { return niveau; }
    double getTaille() const { return taille; }
    std::vector<double> getCentre() const { return centre; }
    std::vector<double> getCentreMasse() const { return centreMasse; }
    double getMasse() const { return masse; }
    Boite* getPremiereFille() const { return premiereFille; }
    Boite* getSoeur() const { return soeur; }
    Particule* getParticule() const { return particule; }
    
    // Setters
    void setSoeur(Boite* s) { soeur = s; }
    void setPremiereFille(Boite* f) { premiereFille = f; }
    
    // Méthodes utilitaires
    bool estTerminale() const;
    bool estVide() const;
    bool contientParticule(const Particule* p) const;
    std::vector<double> calculerDistance(const Boite* autre) const;
    void diviserBoite();
    void nettoyerBoitesVides();
    
private:
    // Méthodes internes
    bool doitDiviser() const;
    int getIndiceSousBoite(const std::vector<double>& position) const;
    std::vector<double> getCentreSousBoite(int indice) const;
};

#endif