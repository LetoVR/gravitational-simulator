#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "Particule.hpp"
#include "Boite.hpp"
#include <string>

// ===============================
// Utilitaires sur le système
// ===============================

// Remet la force de toutes les particules à zéro
void resetForces(Particule* head, int N);

// Enregistre la position actuelle de chaque particule
// dans Particule::liste_position
void enregistrerPositions(Particule* head, int N);

// Intègre le mouvement de toutes les particules
// en appelant Particule::saute_mouton(delta_t)
void integrerSysteme(Particule* head, int N, double delta_t);

// Libère la mémoire des N particules (liste circulaire)
void detruireSysteme(Particule* head, int N);

// (Optionnel mais pratique) Affiche toutes les particules
void printSysteme(Particule* head, int N);

// ===============================
// Gestion de l'arbre de boîtes
// ===============================

// Construit la boîte racine et y insère les N particules
// centre : centre de la boîte racine
// taille : taille de la boîte racine
Boite* construireArbre(Particule* head, int N, const coord& centre, double taille);

// ===============================
// Calcul des forces (Barnes-Hut / quadtree)
// ===============================

// Fonction "wrapper" pour calculer les forces sur toutes les particules
// via l'arbre. Le corps exact dépendra de votre implémentation de Boite.cpp.
void calculerForcesSysteme(Particule* head, int N, Boite* racine, double theta = 0.5);

// ===============================
// Un pas de simulation complet
// ===============================

// Effectue un pas de simulation :
// 1) enregistrement des positions
// 2) reset des forces
// 3) construction de l'arbre
// 4) calcul des forces
// 5) intégration
// 6) destruction de l'arbre
void simulerUnPas(Particule* head,int N,double delta_t,const coord& centreRacine,double tailleRacine,double theta = 0.5);

// ===============================
// Export des résultats
// ===============================

// Exporte les trajectoires au format CSV.
// Format prévu : particle_id,step,x,y (et z... si D > 2)
void exporterTrajectoiresCSV(Particule* head, int N, const std::string& nomFichier);

#endif // SIMULATION_HPP