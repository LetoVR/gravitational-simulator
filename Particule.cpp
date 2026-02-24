#include "Particule.hpp"


using namespace std;

//Constructeur
Particule::Particule(const coord& p, const coord& v, double m, const coord& f, Particule* next)
    : position(p), vitesse(v), force(f), masse_p(m), particule_suivante(next)
{
    liste_position.push_back(position);
}

//  Getters 
double Particule::getMasse() const { return masse_p; }
const coord& Particule::getPosition() const { return position; }
const coord& Particule::getVitesse() const { return vitesse; }
const coord& Particule::getForce() const { return force; }
Particule* Particule::getSuivante() { return particule_suivante; }
const Particule* Particule::getSuivante() const { return particule_suivante; }

//  Méthodes 
void Particule::deplacer(const coord& new_pos) {
    position = new_pos;
}

void Particule::ajouter_position(const coord& pos) {
    liste_position.push_back(pos);
}

void Particule::saute_mouton(double dt) {
    // demi-pas vitesse : v += (F/m) * dt/2
    for (int i = 0; i < D; ++i) {
        vitesse[i] += (force[i] / masse_p) * dt * 0.5;
    }

    // pas position : x += v * dt
    for (int i = 0; i < D; ++i) {
        position[i] += vitesse[i] * dt;
    }

    // Historique (film Matlab)
    ajouter_position(position);

    // demi-pas vitesse (⚠️ en gravitation "exacte", il faudrait recalculer force après le déplacement,
    // puis faire ce demi-pas avec F_{k+1}. Cela se fait généralement dans la boucle du système.)
    for (int i = 0; i < D; ++i) {
        vitesse[i] += (force[i] / masse_p) * dt * 0.5;
    }
}

void Particule::print() const {
    cout << "Position: (";
    for (int i = 0; i < D; ++i) cout << position[i] << (i + 1 < D ? ", " : "");
    cout << "), Vitesse: (";
    for (int i = 0; i < D; ++i) cout << vitesse[i] << (i + 1 < D ? ", " : "");
    cout << "), Masse: " << masse_p << endl;
}

void Particule::print_liste() const {
    const Particule* current = this;

    if (!current) return;

    if (!current->particule_suivante) {
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

// Forces
coord forceEntreParticules(const Particule* p1, const Particule* p2) {
    coord F = zeros();
    if (!p1 || !p2) return F;

    const double G = 1.0;
    const double eps = 1e-12;

    // d = r2 - r1 (vecteur en D dimensions)
    coord d = zeros();
    for (int i = 0; i < D; ++i) {
        d[i] = p2->position[i] - p1->position[i];
    }

    double r2 = norm2(d) + eps;
    double r  = std::sqrt(r2);

    // F = G*m1*m2 / r^3 * d
    double facteur = G * p1->masse_p * p2->masse_p / (r2 * r);

    for (int i = 0; i < D; ++i) {
        F[i] = facteur * d[i];
    }

    return F;
}

coord forceTotaleSurParticule(Particule* particule) {
    coord Ftot = zeros();
    if (!particule || !particule->particule_suivante) return Ftot;

    Particule* courant = particule->particule_suivante;
    while (courant && courant != particule) {
        coord F = forceEntreParticules(particule, courant);
        for (int i = 0; i < D; ++i) {
            Ftot[i] += F[i];
        }
        courant = courant->particule_suivante;
    }

    return Ftot;
}

//  Création du système
Particule* creerSysteme(int N) {
    if (N <= 0) return nullptr;

    // Générateur aléatoire initialisé une seule fois (même si la fonction est appelée plusieurs fois)
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_real_distribution<double> dist(0.0, 1.0);

    // 1ère particule (tête) -> aléatoire aussi
    coord p0 = zeros();
    for (int k = 0; k < D; ++k) {
        p0[k] = dist(gen);
    }

    coord v0 = zeros();
    coord f0 = zeros();
    double m0 = 1.0;

    Particule* head = new Particule(p0, v0, m0, f0, nullptr);
    Particule* courant = head;

    // autres particules
    for (int i = 1; i < N; ++i) {
        coord p = zeros();
        for (int k = 0; k < D; ++k) {
            p[k] = dist(gen);
        }

        coord v = zeros();
        coord f = zeros();
        double m = 1.0;

        Particule* np = new Particule(p, v, m, f, nullptr);
        courant->particule_suivante = np;
        courant = np;
    }

    // fermer la liste circulaire
    courant->particule_suivante = head;

    return head;
}
