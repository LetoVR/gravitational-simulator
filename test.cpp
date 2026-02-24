// Fichier des tests de fonctions

#include "Boite.hpp"
#include "Particule.hpp"

// Facteur d'échelle pour convertir les coordonnées 0-1 en pixels écran
const double SCALE = 800.0;
const int N = 10; // Nombre de particules pour le test
// Paramètres pour petites oscillations pour tester la màj des boites

const double OSC_AMP = 0.09; // amplitude en coordonnées (0-1) -> *SCALE pour pixels
const double OSC_FREQ = 6.0; // fréquence (rad/s)

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

// Dessine les particules en leur appliquant une petite oscillation visuelle
void dessinerParticules(sf::RenderWindow& window, const Particule* p, double t) {
    if (p == nullptr) return;

    sf::CircleShape cercle(5.0f); // Rayon de 5 pixels
    cercle.setFillColor(sf::Color::Red);

    const Particule* courant = p;
    int idx = 0;
    do {
        // Calcul d'un décalage d'affichage sinusoïdal pour rendre les petites oscillations visibles
        double phase = idx * 0.5; // phase légèrement différente par particule
        double offset = OSC_AMP * std::sin(OSC_FREQ * t + phase);

        double drawX = courant->position[0] + offset;
        double drawY = courant->position[1] + offset; // même offset sur x et y pour simplicité

        // Clamp pour rester dans [0,1]
        if (drawX < 0.0) drawX = 0.0;
        if (drawX > 1.0) drawX = 1.0;
        if (drawY < 0.0) drawY = 0.0;
        if (drawY > 1.0) drawY = 1.0;

        cercle.setPosition(drawX * SCALE - 5.0f, drawY * SCALE - 5.0f);
        window.draw(cercle);

        courant = courant->getSuivante();
        ++idx;
    } while (courant && courant != p);
}

int main() {
    const int largeur = 800;
    const int hauteur = 800;
    sf::RenderWindow window(sf::VideoMode(largeur, hauteur), "Simulation Gravitationnelle - Grille");

    // Horloge pour animer les petites oscillations visuelles
    sf::Clock clock;

    // Initialisation de la boîte racine centrée en coordonnées 0-1
    coord centreRacine = {0.5, 0.5};
    Boite racine(0, centreRacine, 1.0); // Niveau 0, taille 1.0, 2D (boîte [0,1]x[0,1])

    // Initialisation de quelques particules pour tester l'affichage
    Particule* systeme = creerSysteme(5); // Crée un système de 10 particules
    Particule * courant = systeme;
    while(courant && courant->getSuivante() && courant->getSuivante() != systeme) {
        racine.ajouterParticule(courant); // Ajoute la particule à la boîte racine
        courant = courant->getSuivante();
     }

    // Boucle principale
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            
            // if (event.type == sf::Event::MouseButtonPressed) {
            //     // 1. Récupération de la position de la souris (conversion en coordonnées 0-1)
            //     sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            //     double mouseX = static_cast<double>(mousePos.x) / SCALE;
            //     double mouseY = static_cast<double>(mousePos.y) / SCALE;
                
                
            //     // 2. Recherche de la boîte sous le curseur
            //     Boite* cible = racine.trouverBoiteTerminale(mouseX, mouseY);

            //     if (cible != nullptr) {
            //         if (event.mouseButton.button == sf::Mouse::Left) {
            //             // Clic gauche : Subdivision
            //             cible->diviserBoite();
            //         } 
            //         else if (event.mouseButton.button == sf::Mouse::Right) {
            //             // Clic droit : Suppression (Nettoyage)
            //             // Note : Pour supprimer une boite spécifique, il faudrait 
            //             // modifier le parent. Ici, on peut tester ton nettoyeur.
            //             // (Nécessite que estVide() soit implémentée)
            //             racine.nettoyerBoitesVides(); 
            //         }
            //     }
            // }
        }

        // --- Phase de calcul (Future mise à jour des forces/positions) ---
        // racine.mettreAJourCentreMasse();
        
        // --- Phase de rendu ---
        window.clear(sf::Color::Black);
        
        // calcul du temps écoulé pour l'animation visuelle
        double t = clock.getElapsedTime().asSeconds();

        dessinerStructureBoite(window, &racine);
        dessinerParticules(window, systeme, t);
        
        window.display();
    }
    return 0;
}
