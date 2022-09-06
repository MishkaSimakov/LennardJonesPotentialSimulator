#ifndef PHYSICSSIMULATION_WORLDDATA_H
#define PHYSICSSIMULATION_WORLDDATA_H

#include <map>

#include "Atom.h"
#include "InteractionInfo.h"

class WorldData {
private:
    int iterations_per_impulse_measurements{500};
    bool m_is_colliding_with_walls{true};
    bool m_is_gravity_enabled{true};
    bool m_is_colliding_with_moving_wall{true};
    double m_dt{0.01};

    std::map<std::pair<AtomType, AtomType>, InteractionInfo> m_interactions{
            {{AtomType::BODY,  AtomType::BODY},  InteractionInfo(48, 1000)},
    };

    sf::Vector2d m_box_size;
public:
    [[nodiscard]] int getIterationsPerImpulseMeasurements() const {
        return iterations_per_impulse_measurements;
    }

    [[nodiscard]] bool isCollidingWithWalls() const {
        return m_is_colliding_with_walls;
    }

    [[nodiscard]] bool isGravityEnabled() const {
        return m_is_gravity_enabled;
    }

    [[nodiscard]] bool isCollidingWithMovingWall() const {
        return m_is_colliding_with_moving_wall;
    }

    [[nodiscard]] double getTimeDelta() const {
        return m_dt;
    }

    [[nodiscard]] const std::map<std::pair<AtomType, AtomType>, InteractionInfo> &getInteractions() const {
        return m_interactions;
    }

    [[nodiscard]] const InteractionInfo &getInteraction(const AtomType &first, const AtomType &second) const {
        return m_interactions.find({first, second})->second;
    }

    [[nodiscard]] const sf::Vector2d &getBoxSize() const {
        return m_box_size;
    }


    void setIterationsPerImpulseMeasurements(int iterationsPerImpulseMeasurements) {
        iterations_per_impulse_measurements = iterationsPerImpulseMeasurements;
    }

    void setIsCollidingWithWalls(bool isCollidingWithWalls) {
        m_is_colliding_with_walls = isCollidingWithWalls;
    }

    void setIsGravityEnabled(bool isGravityEnabled) {
        m_is_gravity_enabled = isGravityEnabled;
    }

    void setIsCollidingWithMovingWall(bool isCollidingWithMovingWall) {
        m_is_colliding_with_moving_wall = isCollidingWithMovingWall;
    }

    void setTimeDelta(double dt) {
        m_dt = dt;
    }

    void setBoxSize(const sf::Vector2d &boxSize) {
        m_box_size = boxSize;
    }
};


#endif //PHYSICSSIMULATION_WORLDDATA_H
