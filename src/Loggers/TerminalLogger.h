#ifndef PHYSICSSIMULATION_TERMINALLOGGER_H
#define PHYSICSSIMULATION_TERMINALLOGGER_H

#include <iostream>

#include "Logger.h"

class TerminalLogger: public Logger {
public:
    void log(const World &world, int iteration) override {
        auto position = world.getAtoms()[0].position;

        std::cout << position.x << " " << position.y << std::endl;
    }
};


#endif //PHYSICSSIMULATION_TERMINALLOGGER_H
