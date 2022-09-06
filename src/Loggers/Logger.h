#ifndef PHYSICSSIMULATION_LOGGER_H
#define PHYSICSSIMULATION_LOGGER_H

#include "World.h"

class Logger {
public:
    virtual void log(const World &world, int iteration) = 0;

    virtual ~Logger() = default;
};


#endif //PHYSICSSIMULATION_LOGGER_H
