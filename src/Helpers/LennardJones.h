#ifndef PHYSICSSIMULATION_LENNARDJONES_H
#define PHYSICSSIMULATION_LENNARDJONES_H

#include <cmath>
#include "InteractionInfo.h"


class LennardJones {
public:
    [[nodiscard]] inline static double getForce(double distance_sqr, const InteractionInfo &info) {
        if (distance_sqr >= 6.25 * info.SIGMA_SQR) return 0;

        return info.COEFF * (std::pow(distance_sqr, 3) - 2 * info.SIGMA_SIXTH_POWER)
               / std::pow(distance_sqr, 7);
    }

    [[nodiscard]] inline static double getWallForce(double distance, const InteractionInfo &info) {
        return 63 * M_PI * info.EPSILON * std::pow(info.SIGMA_SIXTH_POWER, 2) / 256 / std::pow(distance, 11);
    }

    [[nodiscard]] inline static double getPotential(double distance, const InteractionInfo &info) {
        return 4 * info.EPSILON * (std::pow(info.SIGMA / distance, 12) - std::pow(info.SIGMA / distance, 6));
    }
};


#endif //PHYSICSSIMULATION_LENNARDJONES_H
