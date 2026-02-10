#include "PARTICULE.HPP" 
#include <cmath>
#include <random>
 
Particule::Particule(coord p = {0.,0.}, coord v = {0., 0.}, double m = 1., coord f = {0.,0.}, Particule* p_s = nullptr)
    : position(p), vitesse(v), masse_p(m), force(f), particule_suivante(p_s) {
        liste_position.push_back(position);
    }


void Particule::deplacer(const coord& new_pos) { position=new_pos; }

void Particule::ajouter_position(const coord& pos) {
    liste_position.push_back(pos);
}
void Particule::saute_mouton(double delta_t) {
    // Calcul de l'accélération
    double ax = force.x / masse_p;
    double ay = force.y / masse_p;

    // Intégration vitesse (demi-pas)
    vitesse.x += ax * delta_t * 0.5;
    vitesse.y += ay * delta_t * 0.5;

    // Intégration position (pas complet)
    position.x += vitesse.x * delta_t;
    position.y += vitesse.y * delta_t;

    // Intégration vitesse (demi-pas)
    vitesse.x += ax * delta_t * 0.5;
    vitesse.y += ay * delta_t * 0.5;
}

void Particule::print() const {
    cout << "Position: (" << position.x << ", " << position.y << "), "
         << "Vitesse: (" << vitesse.x << ", " << vitesse.y << "), "
         << "Masse: " << masse_p << endl;
}
void Particule::print_liste() const {
    const Particule* current = this;

    // sécurité si liste mal construite
    if (!particule_suivante) {
        current->print();
        cout << "Fin de la liste des particules." << endl;
        return;
    }

    do {
        current->print();
        current = current->particule_suivante;
    } while (current && current != this);

    cout << "Fin de la liste des particules." << endl;
}
// Force exercée par p2 sur p1 (gravitation)
coord forceEntreParticules(const Particule* p1, const Particule* p2) {
    coord F{0.0, 0.0};
    if (!p1 || !p2) return F;

    const double G = 1.0;     // à adapter si nécessaire
    const double eps = 1e-12; // évite division par 0

    double dx = p2->position.x - p1->position.x;
    double dy = p2->position.y - p1->position.y;

    double r2 = dx*dx + dy*dy + eps;
    double r  = std::sqrt(r2);

    // F = G*m1*m2 / r^3 * (dx,dy)
    double facteur = G * p1->masse_p * p2->masse_p / (r2 * r);

    F.x = facteur * dx;
    F.y = facteur * dy;

    return F;
}
// Force totale subie par une particule (somme des forces de toutes les autres)
coord forceTotaleSurParticule(Particule* particule) {
    coord Ftot{0.0, 0.0};
    if (!particule || !particule->particule_suivante) return Ftot;

    Particule* courant = particule->particule_suivante;

    while (courant && courant != particule) {
        coord F = forceEntreParticules(particule, courant);
        Ftot.x += F.x;
        Ftot.y += F.y;
        courant = courant->particule_suivante;
    }

    return Ftot;
}
Particule* creerSysteme(int N) {
    if (N <= 0) return nullptr;

    Particule* p0 = new Particule();
    Particule* courant = p0;

    for (int i = 1; i < N; ++i) {
        Particule* p = new Particule(
            {double(rand()) / RAND_MAX, double(rand()) / RAND_MAX},
            {0.0, 0.0},
            1.0
        );
        courant->particule_suivante = p;
        courant = p;
    }

    courant->particule_suivante = p0; // liste circulaire
    return p0;
}