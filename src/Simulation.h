#ifndef PHYSICSSIMULATION_SIMULATION_H
#define PHYSICSSIMULATION_SIMULATION_H

#include <type_traits>
#include <iostream>

#include "Drawers/Drawer.h"
#include "Loggers/Logger.h"
#include "World.h"

template<class T, class U>
concept Derived = std::is_base_of_v<U, T>;

class Simulation {
private:
    std::unique_ptr<Drawer> m_drawer;
    std::vector<std::unique_ptr<Logger>> m_loggers;
    World m_world;

    const int m_iterations_per_frame{1000};

    int m_iteration{0};
public:
    template<class... Args>
    explicit Simulation(Args... args) :
            m_world(args...) {}

    template<Derived<Drawer> T, class... Args>
    void setDrawer(Args... args) {
        m_drawer = std::make_unique<T>(args...);
    }

    template<Derived<Logger> T, class... Args>
    void addLogger(Args... args) {
        m_loggers.push_back(
                std::make_unique<T>(args...)
        );
    }

    WorldData &getWorldData() {
        return m_world.getWorldData();
    }

    World &getWorld() {
        return m_world;
    }

    void makeSimulationStep() {
        for (int i = 0; i < m_iterations_per_frame; ++i) {
            m_world.makeSimulationStep();
            m_iteration++;
        }
    }

    void writeToLog() {
        for (auto &logger: m_loggers) {
            logger->log(m_world, m_iteration);
        }
    }

    void drawWorld() {
        if (!m_drawer)
            return;

        m_drawer->startDraw();

        for (auto &atom: m_world.getAtoms()) {
            m_drawer->drawAtom(atom);
        }

        m_drawer->endDraw(m_iteration);
    }

    void startSimulation() {
        while (!m_drawer || !m_drawer->wantsToClose()) {
            makeSimulationStep();

            writeToLog();

            drawWorld();
        }
    }

    void startSimulationForIterationsCount(int iterations_count) {
        for (m_iteration = 0; m_iteration < iterations_count;) {
            if (m_drawer && m_drawer->wantsToClose())
                break;

            makeSimulationStep();



            writeToLog();

            drawWorld();
        }
    }
};


#endif //PHYSICSSIMULATION_SIMULATION_H
