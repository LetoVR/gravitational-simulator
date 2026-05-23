#include "Boite.hpp"
#include "Particule.hpp"
#include "raylib.h"

// Constantes d'affichage
const double SCALE = 100.0; // Échelle ajustée pour la 3D
const int N = 200;          // Nombre de particules
const double dt = 0.001;

// Fonction utilitaire pour convertir 'coord' en 'Vector3' de Raylib
Vector3 toRaylibVector(const coord& c) {
    return {
        static_cast<float>(c[0] * SCALE),
        static_cast<float>(c[1] * SCALE),
        static_cast<float>(c[2] * SCALE)
    };
}

// Fonction récursive pour dessiner la structure de l'Octree
void dessinerOctree(const Boite* b) {
    if (b == nullptr) return;

    Vector3 centre = toRaylibVector(b->getCentre());
    float taille = static_cast<float>(b->getTaille() * SCALE);

    // Dessin des arêtes de la boîte (vert transparent)
    DrawCubeWires(centre, taille, taille, taille, Fade(GREEN, 0.3f));

    // Appel récursif sur les filles
    if (!b->estTerminale()) {
        Boite* fille = b->getPremiereFille();
        while (fille != nullptr) {
            dessinerOctree(fille);
            fille = fille->getSoeur();
        }
    }
}

// Parcours de la liste chaînée pour dessiner les particules
void dessinerParticules3D(const Particule* systeme) {
    if (systeme == nullptr) return;

    const Particule* courant = systeme;
    do {
        Vector3 pos = toRaylibVector(courant->getPosition());
        // Sphère rouge avec un rayon fixe
        DrawSphere(pos, 0.5f, RED);
        
        courant = courant->getSuivante();
    } while (courant != systeme && courant != nullptr);
}

int main() {
    // Initialisation de la fenêtre Raylib
    const int screenWidth = 1024;
    const int screenHeight = 768;
    InitWindow(screenWidth, screenHeight, "Simulation N-Corps 3D - Octree");

    // Configuration de la caméra 3D
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 50.0f, 50.0f, 150.0f }; // Position de l'observateur
    camera.target = (Vector3){ 50.0f, 50.0f, 50.0f };    // Point ciblé (centre théorique du système)
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };           // Axe vertical
    camera.fovy = 45.0f;                                 // Champ de vision
    camera.projection = CAMERA_PERSPECTIVE;              // Mode de projection

    SetTargetFPS(60);

    // Initialisation du système physique
    coord centreRacine = {0.5, 0.5, 0.5};
    Boite racine(0, centreRacine, 1.0);    
    Particule* systeme = creerSysteme(N);
    
    // Insertion initiale
    Particule* courantInit = systeme;
    if (courantInit != nullptr) {
        do {
            racine.ajouterParticule(courantInit);
            courantInit = courantInit->getSuivante();
        } while (courantInit != systeme);
    }

    // Boucle principale
    while (!WindowShouldClose()) {
        
        // Mise à jour de la caméra (permet de naviguer avec la souris)
        //UpdateCamera(&camera, CAMERA_ORBITAL);

        // 1. Mise à zéro et calcul des forces
        Particule* courant = systeme;
        if (courant != nullptr) {
            do {
                courant->force = zeros(); 
                racine.calculerForces(courant, THETA);
                courant = courant->getSuivante();
            } while (courant != systeme);
        }

        // 2. Intégration numérique (Saute-mouton)
        courant = systeme;
        if (courant != nullptr) {
            do {
                courant->saute_mouton(dt);
                courant = courant->getSuivante();
            } while (courant != systeme);
        }

        // 3. Mise à jour de la structure spatiale
        racine.mettreAJourArbre();

        // --- Phase de Rendu ---
        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode3D(camera);
                
                //dessinerOctree(&racine);
                dessinerParticules3D(systeme);
                
                if (IsKeyDown(KEY_B)) {
                    dessinerOctree(&racine);
                }

            EndMode3D();

            DrawFPS(10, 10);
            DrawText("Utilisez la souris pour tourner la caméra", 10, 40, 20, LIGHTGRAY);

        EndDrawing();
    }

    // Nettoyage
    detruireSysteme(systeme);
    CloseWindow();

    return 0;
}
