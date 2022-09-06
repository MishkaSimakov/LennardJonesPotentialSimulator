#ifndef PHYSICSSIMULATION_INTERACTIONINFO_H
#define PHYSICSSIMULATION_INTERACTIONINFO_H

#include <cmath>

struct InteractionInfo {
    InteractionInfo() = default;

    InteractionInfo(double sigma, double epsilon) :
            SIGMA(sigma), EPSILON(epsilon),
            SIGMA_SIXTH_POWER(std::pow(sigma, 6)), SIGMA_SQR(sigma * sigma),
            COEFF(-24. * epsilon * std::pow(sigma, 6)) {};

    const double SIGMA {0.};
    const double EPSILON {0.};

    const double SIGMA_SIXTH_POWER {0.};
    const double SIGMA_SQR {0.};

    const double COEFF {0.};
};

#endif //PHYSICSSIMULATION_INTERACTIONINFO_H
