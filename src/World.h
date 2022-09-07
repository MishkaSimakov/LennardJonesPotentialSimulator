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

    WorldData &getWorldData() {
        return m_worldData;
    }

private:
    WorldData m_worldData;

    std::vector<Atom> m_atoms;

    void getForcesForInterval(sf::Vector2d *forces, int begin_index,
                              int end_index) {
        sf::Vector2d f;

        for (int i = begin_index; i < end_index; i++) {
            // atom - atom forces
            for (int j = i + 1; j < m_atoms.size(); j++) {
                auto interaction = m_worldData.getInteraction(m_atoms[i].type, m_atoms[j].type);

                if (std::abs(m_atoms[i].position.x - m_atoms[j].position.x) > 2.5 * interaction.SIGMA)
                    continue;
                if (std::abs(m_atoms[i].position.y - m_atoms[j].position.y) > 2.5 * interaction.SIGMA)
                    continue;

                double distance_sqr = std::pow(m_atoms[i].position.x - m_atoms[j].position.x, 2) +
                                      std::pow(m_atoms[i].position.y - m_atoms[j].position.y, 2);

                f = LennardJones::getForce(distance_sqr, interaction) * (m_atoms[i].position - m_atoms[j].position);

                forces[i] += f;
                forces[j] -= f;
            }

            // gravitation
            if (m_worldData.isGravityEnabled()) {
                double gravity = 0.5;

                forces[i].y -= gravity * m_atoms[i].mass;
            }
        }
    }

    void getForces(sf::Vector2d *forces) {
        std::vector<std::thread> threads;

        unsigned int threads_count = std::thread::hardware_concurrency();
        int count_per_thread = std::floor(m_atoms.size() / threads_count);

        threads.reserve(threads_count);
        for (int i = 0; i < threads_count; i++) {
            threads.emplace_back(
                    &World::getForcesForInterval, this, forces,
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

        // calculate k1
        getForces(k1);

        for (int i = 0; i < m_atoms.size(); ++i) {
            k1[i] *= dt / m_atoms[i].mass;
            m1[i] = m_atoms[i].speed * dt;
        }

        // calculate k2
        for (int i = 0; i < m_atoms.size(); ++i) {
            m_atoms[i].position += m1[i] / 2.;
        }

        getForces(k2);

        for (int i = 0; i < m_atoms.size(); ++i) {
            k2[i] *= dt / m_atoms[i].mass;
            m2[i] = (m_atoms[i].speed + k1[i] / 2.) * dt;
        }

        // calculate k3
        for (int i = 0; i < m_atoms.size(); ++i) {
            m_atoms[i].position -= m1[i] / 2.;
            m_atoms[i].position += m2[i] / 2.;
        }

        getForces(k3);

        for (int i = 0; i < m_atoms.size(); ++i) {
            k3[i] *= dt / m_atoms[i].mass;
            m3[i] = (m_atoms[i].speed + k2[i] / 2.) * dt;
        }

        // calculate k4
        for (int i = 0; i < m_atoms.size(); ++i) {
            m_atoms[i].position -= m2[i] / 2.;
            m_atoms[i].position += m3[i];
        }

        getForces(k4);

        for (int i = 0; i < m_atoms.size(); ++i) {
            k4[i] *= dt / m_atoms[i].mass;
            m4[i] = (m_atoms[i].speed + k3[i]) * dt;
        }

        // calculate final positions
        for (int i = 0; i < m_atoms.size(); ++i) {
            m_atoms[i].position -= m3[i];

            m_atoms[i].position += 1. / 6. * (m1[i] + 2. * m2[i] + 2. * m3[i] + m4[i]);
            m_atoms[i].speed += 1. / 6. * (k1[i] + 2. * k2[i] + 2. * k3[i] + k4[i]);
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
};


#endif //PHYSICSSIMULATION_WORLD_H
