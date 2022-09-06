#ifndef PHYSICSSIMULATION_WORLD_H
#define PHYSICSSIMULATION_WORLD_H

#include "Atom.h"
#include "Helpers/Random.h"
#include "Helpers/LennardJones.h"
#include "Helpers/WorldData.h"

#include <array>
#include <cmath>
#include <iostream>
#include <thread>

class World {
public:
    explicit World(const std::function<void(std::vector<Atom> &)> &atoms_generator) {
        atoms_generator(m_atoms);
    }

    std::vector<Atom> &getAtoms() {
        return m_atoms;
    }

    [[nodiscard]] const std::vector<Atom> &getAtoms() const {
        return m_atoms;
    }

    [[nodiscard]] double getTotalEnergy() const {
        double totalKineticEnergy = 0;

        for (auto &atom: m_atoms) {
            totalKineticEnergy += atom.getKineticEnergy();
        }

        double totalPotentialEnergy = 0;

        for (auto first = m_atoms.begin(); first != m_atoms.end(); first++) {
            for (auto second = first + 1; second != m_atoms.end(); second++) {
                double distance = std::sqrt(
                        std::pow(first->position.x - second->position.x, 2) +
                        std::pow(first->position.y - second->position.y, 2)
                );

                auto interaction = m_worldData.getInteraction(first->type, second->type);
                totalPotentialEnergy += LennardJones::getPotential(distance, interaction);
            }
        }

        return totalKineticEnergy + totalPotentialEnergy;
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
        return m_worldData.getBoxSize().x * getBoxHeight();
    }

    [[nodiscard]] double getDensity() const {
        double totalMass = 0;

        for (auto &atom: m_atoms)
            totalMass += atom.mass;

        return totalMass / getArea();
    }

    [[nodiscard]] double getBoxHeight() const {
        return m_worldData.isCollidingWithMovingWall()
               ? std::min(m_worldData.getBoxSize().y, m_moving_wall_y)
               : m_worldData.getBoxSize().y;
    }

    [[nodiscard]] double getPerimeter() const {
        return 2. * (m_worldData.getBoxSize().x + getBoxHeight());
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

    WorldData &getWorldData() {
        return m_worldData;
    }

private:
    WorldData m_worldData;

    std::vector<Atom> m_atoms;

    double m_pressure{0.};
    double m_total_impulse{0.};

    int m_iteration{0};

    double m_moving_wall_y{0};
    double m_moving_wall_speed{0};
    double m_moving_wall_mass{10.};

    void getForcesForInterval(sf::Vector2d *forces, double *impulse, double *moving_wall_force, int begin_index,
                              int end_index) {
        sf::Vector2d f;

        for (int i = begin_index; i < end_index; i++) {
            // atom - atom forces
            for (int j = i + 1; j < m_atoms.size(); j++) {
                auto interaction = m_worldData.getInteraction(m_atoms[i].type, m_atoms[j].type);

                if (abs(m_atoms[i].position.x - m_atoms[j].position.x) > 2.5 * interaction.SIGMA)
                    continue;
                if (abs(m_atoms[i].position.y - m_atoms[j].position.y) > 2.5 * interaction.SIGMA)
                    continue;

                double distance_sqr = std::pow(m_atoms[i].position.x - m_atoms[j].position.x, 2) +
                                      std::pow(m_atoms[i].position.y - m_atoms[j].position.y, 2);

                f = LennardJones::getForce(distance_sqr, interaction) * (m_atoms[i].position - m_atoms[j].position);

                forces[i] += f;
                forces[j] -= f;
            }

            // atom - wall forces

            if (m_worldData.isCollidingWithWalls()) {
                double wf;
                auto &wall_interaction = m_worldData.getInteraction(m_atoms[i].type, AtomType::WALL);

                // left wall
                wf = LennardJones::getWallForce(m_atoms[i].position.x, wall_interaction);
                forces[i].x += wf;
                *impulse += wf;

                // top wall
                wf = LennardJones::getWallForce(m_atoms[i].position.y, wall_interaction);
                forces[i].y += wf;
                *impulse += wf;

                // right wall
                wf = LennardJones::getWallForce(m_worldData.getBoxSize().x - m_atoms[i].position.x, wall_interaction);
                forces[i].x -= wf;
                *impulse += wf;

                // bottom wall
                wf = LennardJones::getWallForce(getBoxHeight() - m_atoms[i].position.y, wall_interaction);
                forces[i].y -= wf;
                *impulse += wf;
                *moving_wall_force += wf;
            }

            // gravitation
            if (m_worldData.isGravityEnabled()) {
                double gravity = 0.5;

                forces[i].y -= gravity * m_atoms[i].mass;
                *moving_wall_force -= gravity * m_moving_wall_mass;
            }
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
        double dt = m_worldData.getTimeDelta();

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
            k1[i] *= dt / m_atoms[i].mass;
            m1[i] = m_atoms[i].speed * dt;
        }

        impulse1 *= dt;
        mw11 *= dt / m_moving_wall_mass;
        mw21 = m_moving_wall_speed * dt;


        // calculate k2
        for (int i = 0; i < m_atoms.size(); ++i) {
            m_atoms[i].position += m1[i] / 2.;
        }

        getForces(k2, &impulse2, &mw12);

        for (int i = 0; i < m_atoms.size(); ++i) {
            k2[i] *= dt / m_atoms[i].mass;
            m2[i] = (m_atoms[i].speed + k1[i] / 2.) * dt;
        }

        impulse2 *= dt;
        mw12 *= dt / m_moving_wall_mass;
        mw22 = (m_moving_wall_speed + mw11 / 2.) * dt;

        // calculate k3
        for (int i = 0; i < m_atoms.size(); ++i) {
            m_atoms[i].position -= m1[i] / 2.;
            m_atoms[i].position += m2[i] / 2.;
        }

        getForces(k3, &impulse3, &mw13);

        for (int i = 0; i < m_atoms.size(); ++i) {
            k3[i] *= dt / m_atoms[i].mass;
            m3[i] = (m_atoms[i].speed + k2[i] / 2.) * dt;
        }

        impulse3 *= dt;
        mw13 *= dt / m_moving_wall_mass;
        mw23 = (m_moving_wall_speed + mw12 / 2.) * dt;

        // calculate k4
        for (int i = 0; i < m_atoms.size(); ++i) {
            m_atoms[i].position -= m2[i] / 2.;
            m_atoms[i].position += m3[i];
        }

        getForces(k4, &impulse4, &mw14);

        for (int i = 0; i < m_atoms.size(); ++i) {
            k4[i] *= dt / m_atoms[i].mass;
            m4[i] = (m_atoms[i].speed + k3[i]) * dt;
        }

        impulse4 *= dt;
        mw14 *= dt / m_moving_wall_mass;
        mw24 = (m_moving_wall_speed + mw13) * dt;

        // calculate final positions
        for (int i = 0; i < m_atoms.size(); ++i) {
            m_atoms[i].position -= m3[i];

            m_atoms[i].position += 1. / 6. * (m1[i] + 2. * m2[i] + 2. * m3[i] + m4[i]);
            m_atoms[i].speed += 1. / 6. * (k1[i] + 2. * k2[i] + 2. * k3[i] + k4[i]);
        }

        m_total_impulse += 1. / 6. * (impulse1 + 2. * impulse2 + 2. * impulse3 + impulse4);

        if (m_iteration % m_worldData.getIterationsPerImpulseMeasurements() == 0) {
            m_pressure = m_total_impulse / (dt * m_worldData.getIterationsPerImpulseMeasurements()) / getPerimeter();
            m_total_impulse = 0;
        }

        m_moving_wall_y += 1. / 6. * (mw11 + 2. * mw12 + 2. * mw13 + mw14);
        m_moving_wall_speed += 1. / 6. * (mw21 + 2. * mw22 + 2. * mw23 + mw24);

        if (m_worldData.isCollidingWithWalls()) {
            std::erase_if(m_atoms, [&](const Atom &atom) -> bool {
                return isOutside(atom);
            });
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

    bool isOutside(const Atom &atom) {
        if (atom.position.x <= 0 || atom.position.x >= m_worldData.getBoxSize().x)
            return true;

        if (atom.position.y <= 0 || atom.position.y >= getBoxHeight())
            return true;

        return false;
    }

};


#endif //PHYSICSSIMULATION_WORLD_H
