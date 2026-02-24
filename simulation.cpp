#include "simulation.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>

// ======================================================
// Utilitaires sur le système (liste circulaire de N noeuds)
// ======================================================

void resetForces(Particule* head, int N) {
    if (head == nullptr || N <= 0) return;

    Particule* p = head;
    for (int i = 0; i < N; ++i) {
        p->force = zeros();
        p = p->particule_suivante;
    }
}

void enregistrerPositions(Particule* head, int N) {
    if (head == nullptr || N <= 0) return;

    Particule* p = head;
    for (int i = 0; i < N; ++i) {
        p->ajouter_position(p->position);
        p = p->particule_suivante;
    }
}

void integrerSysteme(Particule* head, int N, double delta_t) {
    if (head == nullptr || N <= 0) return;

    Particule* p = head;
    for (int i = 0; i < N; ++i) {
        p->saute_mouton(delta_t);
        p = p->particule_suivante;
    }
}

void detruireSysteme(Particule* head, int N) {
    if (head == nullptr || N <= 0) return;

    Particule* p = head;
    for (int i = 0; i < N; ++i) {
        Particule* next = p->particule_suivante;
        delete p;
        p = next;
    }
}

void printSysteme(Particule* head, int N) {
    if (head == nullptr || N <= 0) {
        std::cout << "[printSysteme] Systeme vide." << std::endl;
        return;
    }

    Particule* p = head;
    for (int i = 0; i < N; ++i) {
        std::cout << "Particule " << i << " : ";
        p->print();
        p = p->particule_suivante;
    }
}

// ======================================================
// Gestion de l'arbre de boîtes
// ======================================================

Boite* construireArbre(Particule* head, int N, const coord& centre, double taille) {
    if (head == nullptr || N <= 0) return nullptr;

    Boite* racine = new Boite(0, centre, taille);

    Particule* p = head;
    for (int i = 0; i < N; ++i) {
        racine->ajouterParticule(p);
        p = p->particule_suivante;
    }

    // Selon votre implémentation, cette ligne peut être utile ou redondante.
    racine->mettreAJourCentreMasse();

    return racine;
}

// ======================================================
// Calcul des forces (wrapper)
// ======================================================
//
// NOTE IMPORTANTE:
// La signature Boite::calculerForces(Boite* autre, double theta) est inhabituelle.
// Sans voir votre Boite.cpp actuel, je ne peux pas garantir l'appel exact attendu.
//
// => Je mets ici une version "fallback" fiable : calcul direct O(N^2)
//    avec forceEntreParticules(p1, p2), en attendant de brancher Barnes-Hut.
//
// Dès que tu m'envoies Boite.cpp, je te remplace ce wrapper par l'appel exact BH.
// ======================================================

void calculerForcesSysteme(Particule* head, int N, Boite* racine, double theta) {
    (void)racine; // évite warning si non utilisé pour l'instant
    (void)theta;

    if (head == nullptr || N <= 0) return;

    // On suppose que resetForces(...) a déjà été appelé avant.
    // Calcul direct de la force totale sur chaque particule.
    Particule* pi = head;
    for (int i = 0; i < N; ++i) {
        coord ftot = zeros();

        Particule* pj = head;
        for (int j = 0; j < N; ++j) {
            if (i != j) {
                coord fij = forceEntreParticules(pi, pj);
                for (int k = 0; k < D; ++k) {
                    ftot[k] += fij[k];
                }
            }
            pj = pj->particule_suivante;
        }

        pi->force = ftot;
        pi = pi->particule_suivante;
    }

    // --------------------------------------------------
    // TODO (version Barnes-Hut du groupe):
    // remplacer le bloc ci-dessus par l'appel correct à vos
    // méthodes de Boite quand Boite.cpp est disponible.
    // Exemple possible (FAUX peut-être selon votre code):
    // racine->calculerForces(racine, theta);
    // --------------------------------------------------
}

// ======================================================
// Un pas de simulation complet
// ======================================================

void simulerUnPas(Particule* head,
                  int N,
                  double delta_t,
                  const coord& centreRacine,
                  double tailleRacine,
                  double theta) {
    if (head == nullptr || N <= 0) return;

    // 1) Sauvegarde des positions (pour export/trajectoires)
    enregistrerPositions(head, N);

    // 2) Reset des forces
    resetForces(head, N);

    // 3) Construction de l'arbre
    Boite* racine = construireArbre(head, N, centreRacine, tailleRacine);

    // 4) Calcul des forces (fallback direct pour l'instant)
    calculerForcesSysteme(head, N, racine, theta);

    // 5) Intégration temporelle
    integrerSysteme(head, N, delta_t);

    // 6) Destruction de l'arbre (reconstruit à chaque pas)
    delete racine;
}

// ======================================================
// Export des trajectoires CSV
// Format: particle_id,step,x,y,(z,... si D>2)
// ======================================================

void exporterTrajectoiresCSV(Particule* head, int N, const std::string& nomFichier) {
    if (head == nullptr || N <= 0) {
        std::cerr << "exporterTrajectoiresCSV: systeme vide." << std::endl;
        return;
    }

    std::ofstream file(nomFichier);
    if (!file.is_open()) {
        std::cerr << "Erreur: impossible d'ouvrir le fichier " << nomFichier << std::endl;
        return;
    }

    // Entête CSV
    file << "particle_id,step";
    for (int k = 0; k < D; ++k) {
        file << ",x" << k;
    }
    file << "\n";

    Particule* p = head;
    for (int id = 0; id < N; ++id) {
        for (std::size_t step = 0; step < p->liste_position.size(); ++step) {
            file << id << "," << step;
            for (int k = 0; k < D; ++k) {
                file << "," << p->liste_position[step][k];
            }
            file << "\n";
        }
        p = p->particule_suivante;
    }

    file.close();
    std::cout << "Trajectoires exportees dans : " << nomFichier << std::endl;
}