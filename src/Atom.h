#ifndef PHYSICSSIMULATION_ATOM_H
#define PHYSICSSIMULATION_ATOM_H

#include <SFML/Graphics.hpp>
#include <cmath>

namespace sf {
    using Vector2d = sf::Vector2<double>;
}

enum class AtomType {
    WALL,
    WATER,
    BODY
};

class Atom {
public:
    sf::Vector2d speed;
    sf::Vector2d position;

    double mass{1.};

    AtomType type {AtomType::BODY};

    Atom() = default;

    [[nodiscard]] double getAbsoluteSpeed() const {
        return std::sqrt(std::pow(speed.x, 2) + std::pow(speed.y, 2));
    }

    [[nodiscard]] double getKineticEnergy() const {
        return mass * std::pow(getAbsoluteSpeed(), 2) / 2.;
    }
};


#endif //PHYSICSSIMULATION_ATOM_H
