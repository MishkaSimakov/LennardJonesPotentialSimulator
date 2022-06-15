#ifndef PHYSICSSIMULATION_WORLD_H
#define PHYSICSSIMULATION_WORLD_H

#include "Atom.h"
#include "Random.h"

#include <array>
#include <cmath>
#include <iostream>
#include <thread>

struct InteractionInfo {
    InteractionInfo() = default;

    InteractionInfo(double sigma, double epsilon) :
            SIGMA(sigma), EPSILON(epsilon),
            SIGMA_SIXTH_POWER(std::pow(sigma, 6)), SIGMA_SQR(sigma * sigma),
            COEFF(-24. * epsilon * std::pow(sigma, 6)) {};

    const double SIGMA {0.};
    const double EPSILON {0.};

    const double SIGMA_SIXTH_POWER {0.};
    const double SIGMA_SQR {0.};

    const double COEFF {0.};
};

class World {
public:
    explicit World(double dt, sf::Vector2d box_size) :
            m_dt(dt), m_box_size(box_size), m_moving_wall_y(box_size.y),
            m_moving_wall_mass(10.) {
        for (int x = 0; x < 100; ++x) {
            for (int y = 0; y < 25; ++y) {
                m_atoms.emplace_back().position = {
                        25 + 2.75 * x + (2.75 / 2.) * (y % 2),
                        25 + 2.75 * y
                };
                m_atoms.back().type = AtomType::WATER;
            }
        }

        for (int x = 0; x < 20; ++x) {
            for (int y = 0; y < 20; ++y) {
                m_atoms.emplace_back().position = {
                        100 + 1.15 * x + (1.15 / 2.) * (y % 2),
                        250 + 1.15 * y
                };
                m_atoms.back().type = AtomType::BODY;
            }
        }


//        for (int x = 500; x < 520; ++x) {
//            for (int y = 1000; y < 1020; ++y) {
//                m_atoms.emplace_back().position = {
//                        start_offset.x + atom_atom_distance * x + (atom_atom_distance / 2.) * (y % 2),
//                        start_offset.y + atom_atom_distance * y
//                };
//            }
//        }
    }

    std::vector<Atom> &getAtoms() {
        return m_atoms;
    }

    void makeSimulationStep() {
        integrate();

        m_iteration++;
    }

    [[nodiscard]] double getTemperature() const {
        double totalKineticEnergy = 0;

        for (auto &atom: m_atoms) {
            totalKineticEnergy += atom.getKineticEnergy();
        }

        return totalKineticEnergy / (double) m_atoms.size();
    }

    [[nodiscard]] double getAverageSpeed() const {
        double totalSpeed = 0;

        for (auto &atom: m_atoms) {
            totalSpeed += atom.getAbsoluteSpeed();
        }

        return totalSpeed / (double) m_atoms.size();
    }

    [[nodiscard]] double getArea() const {
        return m_box_size.x * getBoxHeight();
    }

    [[nodiscard]] double getDensity() const {
        double totalMass = 0;

        for (auto &atom: m_atoms)
            totalMass += atom.mass;

        return totalMass / getArea();
    }

    void increaseTemperature(double value) {
        for (auto &atom: m_atoms)
            atom.speed *= std::sqrt((getTemperature() + value) / getTemperature());
    }

    void setBoxSize(sf::Vector2d size) {
        m_box_size = size;
    }

    [[nodiscard]] const sf::Vector2d &getBoxSize() const {
        return m_box_size;
    }

    void addAtom(Atom &atom) {
        m_atoms.push_back(atom);
    }

    [[nodiscard]] double getBoxHeight() const {
        return m_is_colliding_with_moving_wall ? std::min(m_box_size.y, m_moving_wall_y) : m_box_size.y;
    }

    [[nodiscard]] double getPerimeter() const {
        return 2. * (m_box_size.x + getBoxHeight());
    }

    [[nodiscard]] double getPressure() const {
        return m_pressure;
    }

    [[nodiscard]] double getMovingWallPosition() const {
        return m_moving_wall_y;
    }

    void setMovingWallMass(double mass) {
        m_moving_wall_mass = mass;
    }

    [[nodiscard]] double getMovingWallMass() const {
        return m_moving_wall_mass;
    }

    void collideWithMovingWall(bool collide) {
        m_is_colliding_with_moving_wall = collide;
    }

    [[nodiscard]] bool isCollidingWithMovingWall() const {
        return m_is_colliding_with_moving_wall;
    }

private:
    std::vector<Atom> m_atoms;
    sf::Vector2d m_box_size;

    double m_pressure{0.};
    double m_total_impulse{0.};
    const int ITERATIONS_FOR_IMPULSE_MEASUREMENT{500};

    int m_iteration{0};

    double m_moving_wall_y;
    double m_moving_wall_speed{0};
    double m_moving_wall_mass;
    bool m_is_colliding_with_moving_wall{false};

    // time delta
    double m_dt;

    // constants
    std::map<std::pair<AtomType, AtomType>, InteractionInfo> m_interactions{
            {{AtomType::BODY, AtomType::BODY}, InteractionInfo(1, 100)},
            {{AtomType::WATER, AtomType::WATER}, InteractionInfo(2.725, 4.9115)},

            {{AtomType::WALL, AtomType::WATER}, InteractionInfo(10, 5)},
            {{AtomType::WALL, AtomType::BODY}, InteractionInfo(10, 5)},
            {{AtomType::BODY, AtomType::WATER}, InteractionInfo(2, 0.1)},

            {{AtomType::WATER, AtomType::WALL}, InteractionInfo(10, 5)},
            {{AtomType::BODY, AtomType::WALL}, InteractionInfo(10, 5)},
            {{AtomType::WATER, AtomType::BODY}, InteractionInfo(2, 0.1)},
    };

    void getForcesForInterval(sf::Vector2d *forces, double *impulse, double *moving_wall_force, int begin_index,
                              int end_index) {
        sf::Vector2d f;

        for (int i = begin_index; i < end_index; i++) {
            // atom - atom forces
            for (int j = i + 1; j < m_atoms.size(); j++) {
                if (abs(m_atoms[i].position.x - m_atoms[j].position.x) >
                    2.5 * m_interactions[{m_atoms[i].type, m_atoms[j].type}].SIGMA)
                    continue;
                if (abs(m_atoms[i].position.y - m_atoms[j].position.y) >
                    2.5 * m_interactions[{m_atoms[i].type, m_atoms[j].type}].SIGMA)
                    continue;

                double distance_sqr = std::pow(m_atoms[i].position.x - m_atoms[j].position.x, 2) +
                                      std::pow(m_atoms[i].position.y - m_atoms[j].position.y, 2);

                f = getLennardJonesForce(distance_sqr, m_interactions[{m_atoms[i].type, m_atoms[j].type}]) *
                    (m_atoms[i].position - m_atoms[j].position);

                forces[i] += f;
                forces[j] -= f;
            }

            // atom - wall forces

            double wf;

//            left wall
            wf = getWallForce(m_atoms[i].position.x, m_interactions[{m_atoms[i].type, AtomType::WALL}]);
            forces[i].x += wf;
            *impulse += wf;

//             top wall
            wf = getWallForce(m_atoms[i].position.y, m_interactions[{m_atoms[i].type, AtomType::WALL}]);
            forces[i].y += wf;
            *impulse += wf;

//             right wall
            wf = getWallForce(m_box_size.x - m_atoms[i].position.x, m_interactions[{m_atoms[i].type, AtomType::WALL}]);
            forces[i].x -= wf;
            *impulse += wf;

//             bottom wall
            wf = getWallForce(getBoxHeight() - m_atoms[i].position.y,
                              m_interactions[{m_atoms[i].type, AtomType::WALL}]);
            forces[i].y -= wf;
            *impulse += wf;
            *moving_wall_force += wf;

            // gravitation
            double gravity = 0.5;

            forces[i].y -= gravity * m_atoms[i].mass;
            *moving_wall_force -= gravity * m_moving_wall_mass;
        }
    }

    void getForces(sf::Vector2d *forces, double *impulse, double *moving_wall_force) {
        std::vector<std::thread> threads;

        unsigned int threads_count = std::thread::hardware_concurrency();
        int count_per_thread = std::floor(m_atoms.size() / threads_count);

        threads.reserve(threads_count);
        for (int i = 0; i < threads_count; i++) {
            threads.emplace_back(
                    &World::getForcesForInterval, this, forces, impulse, moving_wall_force,
                    count_per_thread * i,
                    (i == threads_count - 1) ? m_atoms.size() : count_per_thread * (i + 1)
            );
        }

        for (auto &th: threads) {
            th.join();
        }
    }

    void integrate() {
        auto k1 = new sf::Vector2d[m_atoms.size()];
        auto k2 = new sf::Vector2d[m_atoms.size()];
        auto k3 = new sf::Vector2d[m_atoms.size()];
        auto k4 = new sf::Vector2d[m_atoms.size()];

        auto m1 = new sf::Vector2d[m_atoms.size()];
        auto m2 = new sf::Vector2d[m_atoms.size()];
        auto m3 = new sf::Vector2d[m_atoms.size()];
        auto m4 = new sf::Vector2d[m_atoms.size()];

        double impulse1 = 0, impulse2 = 0, impulse3 = 0, impulse4 = 0;

        double mw11 = 0, mw12 = 0, mw13 = 0, mw14 = 0;
        double mw21 = 0, mw22 = 0, mw23 = 0, mw24 = 0;

        // calculate k1
        getForces(k1, &impulse1, &mw11);

        for (int i = 0; i < m_atoms.size(); ++i) {
            k1[i] *= m_dt / m_atoms[i].mass;
            m1[i] = m_atoms[i].speed * m_dt;
        }

        impulse1 *= m_dt;
        mw11 *= m_dt / m_moving_wall_mass;
        mw21 = m_moving_wall_speed * m_dt;


        // calculate k2
        for (int i = 0; i < m_atoms.size(); ++i) {
            m_atoms[i].position += m1[i] / 2.;
        }

        getForces(k2, &impulse2, &mw12);

        for (int i = 0; i < m_atoms.size(); ++i) {
            k2[i] *= m_dt / m_atoms[i].mass;
            m2[i] = (m_atoms[i].speed + k1[i] / 2.) * m_dt;
        }

        impulse2 *= m_dt;
        mw12 *= m_dt / m_moving_wall_mass;
        mw22 = (m_moving_wall_speed + mw11 / 2.) * m_dt;

        // calculate k3
        for (int i = 0; i < m_atoms.size(); ++i) {
            m_atoms[i].position -= m1[i] / 2.;
            m_atoms[i].position += m2[i] / 2.;
        }

        getForces(k3, &impulse3, &mw13);

        for (int i = 0; i < m_atoms.size(); ++i) {
            k3[i] *= m_dt / m_atoms[i].mass;
            m3[i] = (m_atoms[i].speed + k2[i] / 2.) * m_dt;
        }

        impulse3 *= m_dt;
        mw13 *= m_dt / m_moving_wall_mass;
        mw23 = (m_moving_wall_speed + mw12 / 2.) * m_dt;

        // calculate k4
        for (int i = 0; i < m_atoms.size(); ++i) {
            m_atoms[i].position -= m2[i] / 2.;
            m_atoms[i].position += m3[i];
        }

        getForces(k4, &impulse4, &mw14);

        for (int i = 0; i < m_atoms.size(); ++i) {
            k4[i] *= m_dt / m_atoms[i].mass;
            m4[i] = (m_atoms[i].speed + k3[i]) * m_dt;
        }

        impulse4 *= m_dt;
        mw14 *= m_dt / m_moving_wall_mass;
        mw24 = (m_moving_wall_speed + mw13) * m_dt;

        // calculate final positions
        for (int i = 0; i < m_atoms.size(); ++i) {
            m_atoms[i].position -= m3[i];

            m_atoms[i].position += 1. / 6. * (m1[i] + 2. * m2[i] + 2. * m3[i] + m4[i]);
            m_atoms[i].speed += 1. / 6. * (k1[i] + 2. * k2[i] + 2. * k3[i] + k4[i]);
        }

        m_total_impulse += 1. / 6. * (impulse1 + 2. * impulse2 + 2. * impulse3 + impulse4);

        if (m_iteration % ITERATIONS_FOR_IMPULSE_MEASUREMENT == 0) {
            m_pressure = m_total_impulse / (m_dt * ITERATIONS_FOR_IMPULSE_MEASUREMENT) / getPerimeter();
            m_total_impulse = 0;
        }

        m_moving_wall_y += 1. / 6. * (mw11 + 2. * mw12 + 2. * mw13 + mw14);
        m_moving_wall_speed += 1. / 6. * (mw21 + 2. * mw22 + 2. * mw23 + mw24);

        for (auto itr = m_atoms.begin(); itr != m_atoms.end();) {
            if (itr->position.x <= 0 || itr->position.x >= m_box_size.x || itr->position.y <= 0 ||
                itr->position.y >= getBoxHeight())
                itr = m_atoms.erase(itr);
            else
                ++itr;
        }

        delete[] m1;
        delete[] m2;
        delete[] m3;
        delete[] m4;

        delete[] k1;
        delete[] k2;
        delete[] k3;
        delete[] k4;
    }

    [[nodiscard]] inline static double getLennardJonesForce(double distance_sqr, const InteractionInfo &info) {
        if (distance_sqr >= 6.25 * info.SIGMA_SQR) return 0;

        return info.COEFF * (std::pow(distance_sqr, 3) - 2 * info.SIGMA_SIXTH_POWER)
               / std::pow(distance_sqr, 7);
    }

    [[nodiscard]] inline static double getWallForce(double distance, const InteractionInfo &info) {
        return 63 * M_PI * info.EPSILON * std::pow(info.SIGMA_SIXTH_POWER, 2) / 256 / std::pow(distance, 11);
    }
};


#endif //PHYSICSSIMULATION_WORLD_H
