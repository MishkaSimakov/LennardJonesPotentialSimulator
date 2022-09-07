#include "Drawers/WindowDrawer.h"
#include "Loggers/TerminalLogger.h"
#include "Simulation.h"
#include "Drawers/ImageDrawer.h"
#include "Loggers/FileLogger.h"

int main() {
    std::cout.setf(std::ios_base::fixed);

    const std::function<void(std::vector<Atom> &)> generator = [](std::vector<Atom> &atoms) {
        atoms.emplace_back().position = {4.8126, 58.1937};
        atoms.emplace_back().position = {486.755, 461.745};
        atoms.emplace_back().position = {225.389, 335.832};
        atoms.emplace_back().position = {457.552, 339.182};
        atoms.emplace_back().position = {61.5457, 38.2711};
        atoms.emplace_back().position = {67.0528, 190.464};
        atoms.emplace_back().position = {235.298, 106.794};
        atoms.emplace_back().position = {484.179, 200.377};
        atoms.emplace_back().position = {198.063, 112.458};
        atoms.emplace_back().position = {364.978, 383.625};
        atoms.emplace_back().position = {27.0697, 372.986};
        atoms.emplace_back().position = {352.257, 350.083};
        atoms.emplace_back().position = {401.832, 418.837};
        atoms.emplace_back().position = {47.1161, 478.699};
        atoms.emplace_back().position = {371.393, 99.3921};
        atoms.emplace_back().position = {218.593, 268.673};
        atoms.emplace_back().position = {199.978, 333.194};
        atoms.emplace_back().position = {250.296, 216.275};
        atoms.emplace_back().position = {305.471, 369.629};
        atoms.emplace_back().position = {2.2297, 194.264};
        atoms.emplace_back().position = {290.166, 368.012};
        atoms.emplace_back().position = {287.157, 294.501};
        atoms.emplace_back().position = {488.27, 259.863};
        atoms.emplace_back().position = {208.143, 196.897};
        atoms.emplace_back().position = {226.729, 198.795};
    };

    for (int i = 0; i < 10; i++) {
        std::cout << std::setprecision(9) << "Start simulation with dt: " << 1. / pow(10, i) << std::endl;

        Simulation simulation(generator);

        // world settings
        simulation.getWorldData().setTimeDelta(1. / pow(10, i));
        simulation.getWorldData().setIsCollidingWithMovingWall(false);
        simulation.getWorldData().setIsCollidingWithWalls(false);
        simulation.getWorldData().setIsGravityEnabled(false);

        std::cout << std::setprecision(9) << "Start energy: " << simulation.getWorld().getTotalEnergy() << std::endl;
        std::cout << std::endl;

        simulation.addLogger<TerminalLogger>(simulation.getWorld().getTotalEnergy());
        simulation.addLogger<FileLogger>("../log.txt");

        simulation.startSimulationForIterationsCount(100000);

        std::cout << std::endl;
    }

    return 0;
}