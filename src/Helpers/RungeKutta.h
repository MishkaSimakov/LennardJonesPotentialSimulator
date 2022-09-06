#ifndef PHYSICSSIMULATION_RUNGEKUTTA_H
#define PHYSICSSIMULATION_RUNGEKUTTA_H

#include <functional>

template<class T>
class RungeKutta {
protected:
    std::vector<T> k1, k2, k3, k4;
public:
    static double integrate(double xn, double yn, const std::function<double(double, double)> &function, double h) {
        auto k1 = function(xn, yn);
        auto k2 = function(xn + h / 2., yn + h / 2. * k1);
        auto k3 = function(xn + h / 2., yn + h / 2. * k2);
        auto k4 = function(xn + h, yn + h * k3);

        return yn + h / 6. * (k1 + 2 * k2 + 2 * k3 + k4);
    }

    void integrate(
            std::vector<T> &yn,
            const std::function<void(const std::vector<T> &, std::vector<T> &)> &function,
            double h
    ) {
        k1.clear();
        k1.resize(yn.size());

        k2.clear();
        k2.resize(yn.size());

        k3.clear();
        k3.resize(yn.size());

        k4.clear();
        k4.resize(yn.size());


        function(yn, k1);

        for (int i = 0; i < yn.size(); i++) {
            yn[i] += h / 2. * k1[i];
        }

        function(yn, k2);

        for (int i = 0; i < yn.size(); i++) {
            yn[i] -= h / 2. * k1[i];
            yn[i] += h / 2. * k2[i];
        }

        function(yn, k3);

        for (int i = 0; i < yn.size(); i++) {
            yn[i] -= h / 2. * k2[i];
            yn[i] += h * k3[i];
        }

        function(yn, k4);

        for (int i = 0; i < yn.size(); i++) {
            yn[i] -= h * k3[i];

            yn[i] += h / 6. * (k1[i] + 2. * k2[i] + 2. * k3[i] + k4[i]);
        }
    }
};


#endif //PHYSICSSIMULATION_RUNGEKUTTA_H
