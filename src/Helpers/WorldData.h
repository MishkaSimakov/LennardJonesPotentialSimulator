#ifndef PHYSICSSIMULATION_WORLDDATA_H
#define PHYSICSSIMULATION_WORLDDATA_H

#include <map>

#include "Atom.h"
#include "InteractionInfo.h"

class WorldData {
private:
    bool m_is_gravity_enabled{true};
    double m_dt{0.01};

    std::map<std::pair<AtomType, AtomType>, InteractionInfo> m_interactions{
            {{AtomType::BODY,  AtomType::BODY},  InteractionInfo(48, 1000)},
    };
public:
    [[nodiscard]] bool isGravityEnabled() const {
        return m_is_gravity_enabled;
    }

    [[nodiscard]] double getTimeDelta() const {
        return m_dt;
    }

    [[nodiscard]] const std::map<std::pair<AtomType, AtomType>, InteractionInfo> &getInteractions() const {
        return m_interactions;
    }

    [[nodiscard]] const InteractionInfo &getInteraction(const AtomType &first, const AtomType &second) const {
        return m_interactions.find({first, second})->second;
    }

    void setIsGravityEnabled(bool isGravityEnabled) {
        m_is_gravity_enabled = isGravityEnabled;
    }

    void setTimeDelta(double dt) {
        m_dt = dt;
    }
};


#endif //PHYSICSSIMULATION_WORLDDATA_H
