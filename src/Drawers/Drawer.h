#ifndef PHYSICSSIMULATION_DRAWER_H
#define PHYSICSSIMULATION_DRAWER_H


#include "Atom.h"

class Drawer {
public:
    virtual void startDraw() = 0;

    virtual void drawAtom(const Atom &atom) = 0;

    virtual void endDraw(int iteration) = 0;

    [[nodiscard]] virtual bool wantsToClose() const = 0;

    virtual ~Drawer() = default;
};


#endif //PHYSICSSIMULATION_DRAWER_H
