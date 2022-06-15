#include "Window.h"
#include "World.h"
#include "Logger.h"
#include "ImageDrawer.h"


// mac: 25.4716 (250 it)
// pi: 224.402 (250 it)

int main() {
    double dt = 0.005;

    World world(dt, {500, 500});
//    Window window(1600, 1600);
    ImageDrawer imageDrawer({1000, 1000});


    int iteration = 0;
    sf::Vector2d drag_start_position = {0, 0};
    sf::Clock drag_time_clock;

//    enum class DrawingMode {
//        CHART,
//        SIMULATION
//    };

//    DrawingMode mode = DrawingMode::SIMULATION;

//    window.addCallback(sf::Event::KeyReleased, [&](const sf::Event &event) {
//        if (event.key.code == sf::Keyboard::N)
//            world.increaseTemperature(50);
//        else if (event.key.code == sf::Keyboard::C)
//            mode = (mode == DrawingMode::SIMULATION) ? DrawingMode::CHART : DrawingMode::SIMULATION;
//        else if (event.key.code == sf::Keyboard::M)
//            world.setMovingWallMass(world.getMovingWallMass() + 10.);
//        else if (event.key.code == sf::Keyboard::T)
//            world.collideWithMovingWall(!world.isCollidingWithMovingWall());
//    });

//    Logger logger("/Users/mihailsimakov/Documents/Programs/CLionProjects/PhysicsSimulation/log.txt");

    //    window.addCallback(sf::Event::MouseButtonPressed, [&](const sf::Event &event) {
//        if (event.mouseButton.button == sf::Mouse::Left) {
//            drag_start_position = window.getClickCoordinate();
//            drag_time_clock.restart();
//        }
//    });
//    window.addCallback(sf::Event::MouseButtonReleased, [&](const sf::Event &event) {
//        if (event.mouseButton.button == sf::Mouse::Left) {
//            Atom atom;
//            atom.mass = 1.;
//            atom.position = window.getClickCoordinate();
//            atom.speed = (window.getClickCoordinate() - drag_start_position) /
//                         (double) drag_time_clock.restart().asSeconds();
//
//            world.addAtom(atom);
//        }
//    });

    sf::Clock total_clock;

    sf::Clock clock;

    const int iterations_per_frame = 10;

    while (true) {
        clock.restart();

        for (int i = 0; i < iterations_per_frame; ++i) {
            world.makeSimulationStep();
            iteration++;
        }

        std::cout << "Iteration: " << iteration << std::endl;

        std::cout << "Simulation time: " << clock.restart().asSeconds() << std::endl;

//        if (iteration == 1000) {
//            for (int i = world.getAtoms().size() - 400; i < world.getAtoms().size(); ++i) {
//                world.getAtoms()[i].speed = {0, -50};
//            }
//        }

//        world.setMovingWallMass(world.getMovingWallMass() + 0.01);

//        world.increaseTemperature(1);

//        world.setBoxSize(
//                world.getBoxSize() - sf::Vector2d{0., 0.5}
//        );
//
//        if (world.getBoxSize().y < 25)
//            break;

//        window.pollEvents();
//
//        window.handlePressedKeys();
//
//        window.startDraw();

//        std::vector<double> speeds;
//        speeds.reserve(world.getAtoms().size());

//        for (auto &atom: world.getAtoms())
//            speeds.emplace_back(atom.getAbsoluteSpeed());
//
//        if (mode == DrawingMode::CHART) {
//            window.drawSpeedDistribution(speeds);
//        } else {
//
//            window.drawCoordinateAxes();
//            window.drawBorders(world.getBoxSize());
//
//            if (world.isCollidingWithMovingWall())
//                window.drawMovingWall(world.getMovingWallPosition(), world.getBoxSize());
//
//            for (auto &atom: world.getAtoms()) {
//                window.drawAtom(atom);
//            }
//
//            window.drawDebugInfo({
//                                         {"Iteration",      std::to_string(iteration)},
//                                         {"Temperature",    std::to_string(world.getTemperature())},
//                                         {"Pressure",       std::to_string(world.getPressure())},
//                                         {"Time per frame", std::to_string(clock.restart().asMilliseconds())},
//                                 });
//        }
//
//        window.endDraw();

        clock.restart();

        imageDrawer.startDraw();

        for (auto &atom: world.getAtoms())
            imageDrawer.drawAtom(atom, world.getBoxSize(), 5);

//        imageDrawer.drawBorders(world.getBoxSize());

        imageDrawer.endDraw(
                "/Users/mihailsimakov/Documents/Programs/CLionProjects/PhysicsSimulation/images/" +
                std::to_string(iteration) + ".png"
        );

        std::cout << "Drawing time: " << clock.restart().asSeconds() << std::endl;

        std::cout << "Time: " << dt * iteration << std::endl;
        std::cout << "Atoms count: " << world.getAtoms().size() << std::endl;

//        logger.log(world.getTemperature(), world.getPressure(), world.getArea(), world.getAtoms().size());

        std::cout << std::endl;
    }

    std::cout << "Total time: " << total_clock.restart().asSeconds() << std::endl;

    return 0;
}
