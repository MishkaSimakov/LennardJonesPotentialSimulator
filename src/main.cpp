#include "Drawers/WindowDrawer.h"
#include "Loggers/TerminalLogger.h"
#include "Simulation.h"

int main() {
    const std::function<void(std::vector<Atom> &)> generator = [](std::vector<Atom> &atoms) {
        atoms.emplace_back().position = {0, 0};
        atoms.emplace_back().position = {50, 0};
    };

    Simulation simulation(generator);

    // world settings
    simulation.getWorldData().setTimeDelta(0.01);
    simulation.getWorldData().setIsCollidingWithMovingWall(false);
    simulation.getWorldData().setIsCollidingWithWalls(false);
    simulation.getWorldData().setIsGravityEnabled(false);

    simulation.setDrawer<WindowDrawer>(1000, 1000);

    simulation.setLogger<TerminalLogger>();

    simulation.startSimulation();

    return 0;
}