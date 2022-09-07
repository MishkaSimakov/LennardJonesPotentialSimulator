#ifndef PHYSICSSIMULATION_TERMINALLOGGER_H
#define PHYSICSSIMULATION_TERMINALLOGGER_H

#include <iostream>
#include <iomanip>

#include "Logger.h"

class TerminalLogger : public Logger {
protected:
    double m_energy;
public:
    explicit TerminalLogger(double energy) : m_energy(energy) {}

    void log(const World &world, int iteration) override {
        double current_energy = world.getTotalEnergy();

        std::cout << std::setprecision(9) << iteration << ": "
                  << "energy: " << current_energy
                  << "; delta (%): " << (current_energy - m_energy) / m_energy * 100 << std::endl;
    }
};


#endif //PHYSICSSIMULATION_TERMINALLOGGER_H
