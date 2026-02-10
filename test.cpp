// Fichier des tests de fonctions

#include "Boite.hpp"

void dessinerStructureBoite(sf::RenderWindow& window, const Boite* b) {
    if (b == nullptr) return;

    // Récupération des propriétés de la boîte
    std::vector<double> centre = b->getCentre();
    double taille = b->getTaille();

    // Configuration de la forme (carré)
    sf::RectangleShape rectangle(sf::Vector2f(taille, taille));
    // SFML utilise le coin supérieur gauche, on convertit depuis le centre
    rectangle.setPosition(centre[0] - taille / 2.0, centre[1] - taille / 2.0);
    
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

int main() {
    const int largeur = 800;
    const int hauteur = 800;
    sf::RenderWindow window(sf::VideoMode(largeur, hauteur), "Simulation Gravitationnelle - Grille");

    // Initialisation de la boîte racine centrée
    std::vector<double> centreRacine = {largeur / 2.0, hauteur / 2.0};
    Boite racine(0, centreRacine, 600.0, 2); // Niveau 0, taille 600, 2D

    // Boucle principale
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            
            // // Interaction de test : Diviser la boîte au clic
            // if (event.type == sf::Event::MouseButtonPressed) {
            //     racine.diviserBoite();
            // }


            if (event.type == sf::Event::MouseButtonPressed) {
            // 1. Récupération de la position de la souris
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            double mouseX = static_cast<double>(mousePos.x);
            double mouseY = static_cast<double>(mousePos.y);
            
            
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
                    // (Nécessite que estVide() soit implémenté)
                    racine.nettoyerBoitesVides(); 
                }
            }
        }
    }

    // --- Phase de calcul (Future mise à jour des forces/positions) ---
    // racine.mettreAJourCentreMasse();
    // --- Phase de rendu ---
    window.clear(sf::Color::Black);
    
    dessinerStructureBoite(window, &racine);
    
    window.display();
    }

    return 0;
}