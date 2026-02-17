// Fichier des tests de fonctions

#include "Boite.hpp"
#include "Particule.hpp"

// Facteur d'échelle pour convertir les coordonnées 0-1 en pixels écran
const double SCALE = 800.0;

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

    // Initialisation de la boîte racine centrée en coordonnées 0-1
    coord centreRacine = {0.5, 0.5};
    Boite racine(0, centreRacine, 1.0); // Niveau 0, taille 1.0, 2D (boîte [0,1]x[0,1])

    // Initialisation de quelques particules pour tester l'affichage
    Particule* systeme = creerSysteme(100); // Crée un système de 10 particules

    // Boucle principale
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // petites oscillations des particules pour tester le rendu
            Particule* courant = systeme;
            while (courant && courant->getSuivante() && courant->getSuivante() != systeme) {
                for (int i = 0; i < D; ++i) {
                    courant->position[i] += 0.01 * (rand() / double(RAND_MAX) - 0.5); // Petite oscillation aléatoire
                }
                courant = courant->getSuivante();
            }
            
            if (event.type == sf::Event::MouseButtonPressed) {
            // 1. Récupération de la position de la souris (conversion en coordonnées 0-1)
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            double mouseX = static_cast<double>(mousePos.x) / SCALE;
            double mouseY = static_cast<double>(mousePos.y) / SCALE;
            
            
            // 2. Recherche de la boîte sous le curseur
            Boite* cible = racine.trouverBoiteTerminale(mouseX, mouseY);

            if (cible != nullptr) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    // Clic gauche : Subdivision
                    cible->diviserBoite();
                } 
                else if (event.mouseButton.button == sf::Mouse::Right) {
                    // Clic droit : Suppression (Nettoyage)
                    // Note : Pour supprimer une boite spécifique, il faudrait 
                    // modifier le parent. Ici, on peut tester ton nettoyeur.
                    // (Nécessite que estVide() soit implémentée)
                    racine.nettoyerBoitesVides(); 
                }
            }
        }

        // --- Phase de calcul (Future mise à jour des forces/positions) ---
        // racine.mettreAJourCentreMasse();
        
        // --- Phase de rendu ---
        window.clear(sf::Color::Black);
        
        dessinerStructureBoite(window, &racine);
        dessinerParticules(window, systeme);
        
        window.display();
        }

    
    }
    return 0;
}