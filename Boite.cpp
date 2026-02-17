#include "Boite.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>

using namespace std;

// Constructeur
Boite::Boite(int niv, coord cent, double t) 
    : niveau(niv), centre(cent), taille(t) {
    
    centreMasse = zeros();
    masse = 0.0;
    //particule = nullptr;
    premiereFille = nullptr;
    soeur = nullptr;
}

// Destructeur
Boite::~Boite() {
    delete premiereFille;
    delete soeur;
    // Note: On ne détruit pas la particule ici car elle est gérée ailleurs
}

// Vérifie si la boîte est terminale (contient une particule ou est vide)
bool Boite::estTerminale() const {
    return (premiereFille == nullptr);
}

// Vérifie si la boîte est vide
bool Boite::estVide() const {
    return ( particule == nullptr && premiereFille == nullptr);
}

// Ajout d'une particule (procédure récursive)
void Boite::ajouterParticule(Particule* p) {
    // Cas 1: Boîte terminale vide
    if (estTerminale() && particule == nullptr) {
        particule = p;
        masse = p->getMasse();   // getMasse et getPosition à définir par Jude
        centreMasse = p->getPosition();
        return;
    }
    
    // Cas 2: Boîte terminale déjà occupée
    if (estTerminale() && particule != nullptr) {
        // Sauvegarde de l'ancienne particule
        Particule* ancienneParticule = particule;
        particule = nullptr;
        
        // Division de la boîte
        diviserBoite();
        
        // Ajout des deux particules
        ajouterParticule(ancienneParticule);
        ajouterParticule(p);
        return;
    }
    
    // Cas 3: Boîte avec sous-boîtes
    if (!estTerminale()) {
        int indice = getIndiceSousBoite(p->getPosition());
        
        // Parcours des sous-boîtes pour trouver la bonne
        Boite* sousBoite = premiereFille;
        for (int i = 0; i < indice && sousBoite != nullptr; i++) {
            sousBoite = sousBoite->getSoeur();
        }
        
        if (sousBoite != nullptr) {
            sousBoite->ajouterParticule(p);
            
            // Mise à jour du centre de masse et de la masse
            mettreAJourCentreMasse();
        }
    }
}

// Suppression d'une particule
void Boite::supprimerParticule(Particule* p) {
    // Si la particule est dans cette boîte terminale
    if (estTerminale() && particule == p) {
        particule = nullptr;
        masse = 0.0;
        centreMasse = zeros();
        return;
    }
    
    // Si la boîte a des sous-boîtes
    if (!estTerminale()) {
        Boite* sousBoite = premiereFille;
        while (sousBoite != nullptr) {
            sousBoite->supprimerParticule(p);
            sousBoite = sousBoite->getSoeur();
        }
        
        // Nettoyage des sous-boîtes vides
        nettoyerBoitesVides();
        mettreAJourCentreMasse();
    }
}

// Calcul des forces entre boîtes
void Boite::calculerForces(Boite* autre, double theta) {
    // Si cette boîte est vide, on ne fait rien
    if (estVide()) return;
    
    // Si l'autre boîte est vide, on ne fait rien
    if (autre->estVide()) return;
    
    // Calcul de la distance entre les centres de masse
    double dist = calculerDistance(autre);
    
    // Critère d'approximation
    if (dist > theta * (taille + autre->getTaille())) {
        // Formule exacte pour les particules proches
        if (estTerminale() && autre->estTerminale() && particule != nullptr && 
            autre->getParticule() != nullptr && particule != autre->getParticule()) {
            
            // Calcul de la force gravitationnelle
            coord pos1 = particule->getPosition();
            coord pos2 = autre->getParticule()->getPosition();
            
            double r2 = 0.0;
            for (int i = 0; i < D; i++) {
                double diff = pos1[i] - pos2[i];
                r2 += diff * diff;
            }
            
            // Éviter la singularité (ε² dans la formule)
            double epsilon = 0.01;
            double force = particule->getMasse() * autre->getParticule()->getMasse() / 
                          (r2 + epsilon * epsilon);
            
            // Ici, on doit mettre à jour la force appliquée à la particule ( à discuter avec Jude)
        }
    } else {
        // Formule approchée pour les boîtes lointaines
        if (estTerminale() && particule != nullptr) {
            double force = particule->getMasse() * autre->getMasse() / 
                          (dist * dist);
            
            // Mise à jour approximative de la force
            // À adapter selon votre implémentation
        }
        
        // Appel récursif sur les sous-boîtes
        if (!autre->estTerminale()) {
            Boite* sousBoite = autre->getPremiereFille();
            while (sousBoite != nullptr) {
                calculerForces(sousBoite, theta);
                sousBoite = sousBoite->getSoeur();
            }
        }
    }
}

// Division d'une boîte en sous-boîtes
void Boite::diviserBoite() {
    if (premiereFille != nullptr) return; // Déjà divisée
    
    double nouvelleTaille = taille / 2.0;
    
    // Création des sous-boîtes (8 en 3D, 4 en 2D)
    int nbSousBoites = (D == 3) ? 8 : 4;
    
    Boite* precedent = nullptr;
    for (int i = 0; i < nbSousBoites; i++) {
        coord nouveauCentre = getCentreSousBoite(i);
        Boite* nouvelleBoite = new Boite(niveau + 1, nouveauCentre, nouvelleTaille);
        
        if (i == 0) {
            premiereFille = nouvelleBoite;
        } else {
            precedent->setSoeur(nouvelleBoite);
        }
        precedent = nouvelleBoite;
    }
}

// Obtention de l'indice de la sous-boîte pour une position donnée
int Boite::getIndiceSousBoite(const coord& position) const {
    int indice = 0;
    for (int i = 0; i < D; i++) {
        if (position[i] > centre[i]) {
            indice |= (1 << i);
        }
    }
    return indice;
}

// Calcul du centre d'une sous-boîte
coord Boite::getCentreSousBoite(int indice) const {
    coord nouveauCentre = centre;
    double demiTaille = taille / 4.0; // Car taille/2 pour le déplacement
    
    for (int i = 0; i < D; i++) {
        if (indice & (1 << i)) {
            nouveauCentre[i] += demiTaille;
        } else {
            nouveauCentre[i] -= demiTaille;
        }
    }
    return nouveauCentre;
}

// Mise à jour du centre de masse
void Boite::mettreAJourCentreMasse() {
    if (estTerminale()) {
        if (particule != nullptr) {
            centreMasse = particule->getPosition();
            masse = particule->getMasse();
        } else {
            centreMasse = zeros();
            masse = 0.0;
        }
        return;
    }
    
    // Calcul à partir des sous-boîtes
    double masseTotale = 0.0;
    coord centreMasseTotal = zeros();
    
    Boite* sousBoite = premiereFille;
    while (sousBoite != nullptr) {
        if (sousBoite->getMasse() > 0) {
            double m = sousBoite->getMasse();
            masseTotale += m;
            
            coord cm = sousBoite->getCentreMasse();
            for (int i = 0; i < D; i++) {
                centreMasseTotal[i] += m * cm[i];
            }
        }
        sousBoite = sousBoite->getSoeur();
    }
    
    if (masseTotale > 0) {
        masse = masseTotale;
        for (int i = 0; i < D; i++) {
            centreMasse[i] = centreMasseTotal[i] / masseTotale;
        }
    } else {
        masse = 0.0;
        centreMasse = zeros();
    }
}

// Nettoyage des sous-boîtes vides
void Boite::nettoyerBoitesVides() {
    if (premiereFille == nullptr) return;
    
    // Comptage des sous-boîtes non vides
    int nbNonVides = 0;
    Boite* sousBoite = premiereFille;
    while (sousBoite != nullptr) {
        if (!sousBoite->estVide()) {
            nbNonVides++;
        }
        sousBoite = sousBoite->getSoeur();
    }
    
    // Si toutes les sous-boîtes sont vides, on les supprime
    if (nbNonVides == 0) {
        delete premiereFille;
        premiereFille = nullptr;
    }
}

// Calcul de la distance entre deux boîtes
double Boite::calculerDistance(const Boite* autre) const {
    coord dist = zeros();
    coord cm1 = centreMasse;
    coord cm2 = autre->getCentreMasse();
    
    for (int i = 0; i < D; i++) {
        dist[i] = cm1[i] - cm2[i];
    }
    
    return norm2(dist);
}   

// Pour l'affichage test 
bool Boite::contient(double x, double y) const {
    return (x >= centre[0] - taille / 2.0 && x <= centre[0] + taille / 2.0 &&
            y >= centre[1] - taille / 2.0 && y <= centre[1] + taille / 2.0);
}

Boite* Boite::trouverBoiteTerminale(double x, double y) {
    // Si le point n'est pas dans cette boîte, il n'est pas dans ses filles
    if (!contient(x, y)) return nullptr;

    // Si c'est une boîte terminale et qu'elle contient le point, c'est la cible
    if (estTerminale()) return this;

    // Sinon, on cherche récursivement dans les filles
    Boite* fille = premiereFille;
    while (fille != nullptr) {
        Boite* cible = fille->trouverBoiteTerminale(x, y);
        if (cible != nullptr) return cible;
        fille = fille->getSoeur();
    }
    return nullptr;
}