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
    particule = nullptr;
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
        masse = p->getMasse();
        centreMasse = p->getPosition();
        return;
    }
    
    // Cas 2: Boîte terminale déjà occupée
    if (estTerminale() && particule != nullptr) {
        
        // Arrêt forcé si la limite de profondeur est atteinte
        if (niveau >= NIVEAU_MAX) {
            return; 
        }

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


// Calcul des forces de tout l'arbre sur une particule donnée
void Boite::calculerForces(Particule* p, double theta) const{
    if (estVide()) return;

    // Calcul de la distance d entre la particule cible p et le centre de masse de la boîte
    coord diff = zeros();
    double d2 = 0.0;
    for (int i = 0; i < D; ++i) {
        diff[i] = centreMasse[i] - p->position[i];
        d2 += diff[i] * diff[i];
    }
    double d = std::sqrt(d2);

    // Sécurité : éviter l'auto-interaction si on évalue la boîte contenant la particule elle-même
    if (d == 0.0) return;

    // Cas 1 : Boîte terminale (interaction exacte particule à particule)
    if (estTerminale()) {
        if (particule != nullptr && particule != p) {
            // Force exacte avec epsilon
            double forceMagnitude = (G * p->getMasse() * masse) / (d2 + (eps * eps));
            // Ajout vectoriel (projection sur les axes x, y...)
            for (int i = 0; i < D; ++i) {
                p->force[i] += forceMagnitude * (diff[i] / d);
            }
        }
    } 
    // Cas 2 : Nœud parent (interactions groupées ou approfondissement de l'arbre)
    else {
        // Application du Multipole Acceptance Criterion (MAC)
        if (taille / d < theta) {
            // Approximation : la boîte est vue comme une masse ponctuelle lointaine
            double forceMagnitude = (G * p->getMasse() * masse) / (d2 + (eps * eps));
            for (int i = 0; i < D; ++i) {
                p->force[i] += forceMagnitude * (diff[i] / d);
            }
        } else {
            // La boîte est trop proche, on l'ouvre et on inspecte ses sous-boîtes
            Boite* fille = premiereFille;
            while (fille != nullptr) {
                fille->calculerForces(p, theta);
                fille = fille->getSoeur();
            }
        }
    }
}
/*
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
*/


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

// Pour identifier les particules qui viennent de sortir de leur boîte.
bool Boite::contient(const coord& p) const {
    for (int i = 0; i < D; ++i) {
        if (p[i] < centre[i] - taille / 2.0 || p[i] > centre[i] + taille / 2.0) {
            return false; // Hors de la boîte sur cet axe
        }
    }
    return true; // Dans la boîte sur tous les axes
}

Boite* Boite::trouverBoiteTerminale(const coord& p) {
    if (!contient(p)) return nullptr;

    if (estTerminale()) return this;

    Boite* fille = premiereFille;
    while (fille != nullptr) {
        Boite* cible = fille->trouverBoiteTerminale(p);
        if (cible != nullptr) return cible;
        fille = fille->getSoeur();
    }
    return nullptr;
}

// Parcours récursif pour trouver les particules sorties de leur boîte
void Boite::recupererParticulesDeplacees(std::vector<Particule*>& a_deplacer) {
    if (estTerminale()) {
        // Remplacer l'ancienne condition par celle-ci :
        if (particule != nullptr && !contient(particule->position)) {
            a_deplacer.push_back(particule);
        }
    } else {
        Boite* fille = premiereFille;
        while (fille != nullptr) {
            fille->recupererParticulesDeplacees(a_deplacer);
            fille = fille->getSoeur();
        }
    }
}

// Fonction globale de mise à jour à appeler depuis le main
void Boite::mettreAJourArbre() {
    std::vector<Particule*> a_deplacer;
    
    // 1. Lister les particules hors limites
    recupererParticulesDeplacees(a_deplacer);
    
    // 2. Déplacer dynamiquement les particules concernées
    for (Particule* p : a_deplacer) {
        supprimerParticule(p);
        ajouterParticule(p);
    }
    
    // 3. Recalculer les centres de masse de l'ensemble de l'arbre
    mettreAJourCentreMasse();
}