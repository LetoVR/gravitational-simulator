// Fichier des tests de fonctions

#include "Boite.hpp"
#include "Particule.hpp"

// Facteur d'échelle pour convertir les coordonnées 0-1 en pixels écran
const double SCALE = 800.0;
const int N = 20; // Nombre de particules pour le test

void dessinerStructureBoite(sf::RenderWindow& window, const Boite* b) {
    if (b == nullptr) return;

    // Récupération des propriétés de la boîte
    coord centre = b->getCentre();
    double taille = b->getTaille();

    // Configuration de la forme (carré)
    sf::RectangleShape rectangle(sf::Vector2f(taille * SCALE, taille * SCALE));
    // SFML utilise le coin supérieur gauche, on convertit depuis le centre
    rectangle.setPosition((centre[0] - taille / 2.0) * SCALE, (centre[1] - taille / 2.0) * SCALE);
    
    // Style : Bordures uniquement pour voir la grille
    rectangle.setFillColor(sf::Color::Transparent);
    rectangle.setOutlineColor(sf::Color::Green);
    rectangle.setOutlineThickness(1.0f);

    window.draw(rectangle);

    // Si la boîte n'est pas terminale, on dessine les filles
    if (!b->estTerminale()) {
        Boite* fille = b->getPremiereFille();
        while (fille != nullptr) {
            dessinerStructureBoite(window, fille);
            fille = fille->getSoeur();
        }
    }
}

void dessinerParticules(sf::RenderWindow& window, const Particule* p) {
    if (p == nullptr) return;

    // Dessiner la particule courante
    sf::CircleShape cercle(5.0f); // Rayon de 5 pixels
    cercle.setFillColor(sf::Color::Red);
    cercle.setPosition(p->position[0] * SCALE - 5.0f, p->position[1] * SCALE - 5.0f); // Centrer le cercle
    window.draw(cercle);

    // Dessiner les particules suivantes dans la liste
    const Particule* suivant = p->getSuivante();
    while (suivant != nullptr && suivant != p) {
        cercle.setPosition(suivant->position[0] * SCALE - 5.0f, suivant->position[1] * SCALE - 5.0f);
        window.draw(cercle);
        suivant = suivant->getSuivante();
    }
}

int main() {
    const int largeur = 800;
    const int hauteur = 800;
    sf::RenderWindow window(sf::VideoMode(largeur, hauteur), "Simulation Gravitationnelle - Grille");

    // Conservation du centre de la racine
    coord centreRacine = {0.5, 0.5};

    Particule* systeme = creerSysteme(N);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // 1. Déplacement des particules (parcours complet de la liste circulaire)
        Particule* courant = systeme;
        if (courant != nullptr) {
            do {
                for (int i = 0; i < D; ++i) {
                    courant->position[i] += 0.01 * (rand() / double(RAND_MAX) - 0.5); 
                }
                courant = courant->getSuivante();
            } while (courant != systeme);
        }
        
        // 1.5 Evite les chevauchements visuels
        const double DIAMETRE_GRAPHIQUE = 0.0125;
        Particule* p1 = systeme;
        if (p1 != nullptr) {
            do {
                Particule* p2 = p1->getSuivante();
                while (p2 != nullptr && p2 != systeme) {
                    coord diff = zeros();
                    double dist2 = 0.0;
                    for (int i = 0; i < D; ++i) {
                        diff[i] = p1->position[i] - p2->position[i];
                        dist2 += diff[i] * diff[i];
                    }
                    
                    if (dist2 > 0 && dist2 < DIAMETRE_GRAPHIQUE * DIAMETRE_GRAPHIQUE) {
                        double dist = std::sqrt(dist2);
                        double chevauchement = DIAMETRE_GRAPHIQUE - dist;
                        
                        // Séparation symétrique pour repousser les deux particules
                        for (int i = 0; i < D; ++i) {
                            double decalage = (diff[i] / dist) * (chevauchement * 0.5);
                            p1->position[i] += decalage;
                            p2->position[i] -= decalage;
                        }
                    }
                    p2 = p2->getSuivante();
                }
                p1 = p1->getSuivante();
            } while (p1 != systeme);
        }

        // 2. Réinitialisation et reconstruction de l'arbre à chaque itération
        Boite racine(0, centreRacine, 1.0);
        courant = systeme;
        if (courant != nullptr) {
            do {
                racine.ajouterParticule(courant);
                courant = courant->getSuivante();
            } while (courant != systeme);
        }

        // --- Phase de rendu ---
        window.clear(sf::Color::Black);
        
        dessinerStructureBoite(window, &racine);
        dessinerParticules(window, systeme);
        
        window.display();
    }
    
    detruireSysteme(systeme); // Libération de la mémoire allouée pour les particules
    return 0;
}
