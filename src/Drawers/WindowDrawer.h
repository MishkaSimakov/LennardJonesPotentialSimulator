#ifndef PHYSICSSIMULATION_WINDOWDRAWER_H
#define PHYSICSSIMULATION_WINDOWDRAWER_H

#include "Atom.h"
#include "Drawer.h"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <map>
#include <functional>
#include <vector>

class WindowDrawer: public Drawer {
private:
    sf::RenderWindow m_window;
    sf::Event m_event{};
    sf::View m_view;
    sf::Clock m_dt_clock;
    sf::Time m_dt;
    std::map<sf::Event::EventType, std::function<void(const sf::Event &)>> m_callbacks;
    sf::Font m_font;

    float m_camera_movement_speed{100.f};
public:
    WindowDrawer(unsigned int width, unsigned int height)
            : m_window({width, height}, "Particles"),
              m_view() {
        m_view.setSize((float) width, (float) height);
        m_view.setCenter(250, 250);
//        m_view.setRotation(180);
        m_view.zoom(1);

        m_window.setView(m_view);

        m_font.loadFromFile("fonts/arial.ttf");
    };

    [[nodiscard]] bool wantsToClose() const override {
        return !m_window.isOpen();
    }

    void startDraw() override {
        pollEvents();

        m_window.clear(sf::Color::White);
        m_dt = m_dt_clock.restart();
    };

    void endDraw(int iteration) override {
        m_window.display();
    }

    void pollEvents() {
        while (m_window.pollEvent(m_event)) {
            switch (m_event.type) {
                case sf::Event::Closed:
                    m_window.close();
                    break;
                case sf::Event::KeyPressed:
                    if (m_event.key.code == sf::Keyboard::Escape)
                        m_window.close();

                    break;
                default:
                    break;
            }

            if (m_callbacks.count(m_event.type))
                m_callbacks[m_event.type](m_event);
        }
    }

    void handlePressedKeys() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            m_view.move({-m_camera_movement_speed * m_dt.asSeconds(), 0});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            m_view.move({0, m_camera_movement_speed * m_dt.asSeconds()});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            m_view.move({m_camera_movement_speed * m_dt.asSeconds(), 0});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            m_view.move({0, -m_camera_movement_speed * m_dt.asSeconds()});

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            m_view.zoom(std::pow(1.2f, m_dt.asSeconds()));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            m_view.zoom(std::pow(0.8f, m_dt.asSeconds()));

        m_window.setView(m_view);
    }

    void drawAtom(const Atom &atom) override {
        float radius = 5.f;

        sf::CircleShape atom_shape;

        atom_shape.setRadius(radius);
        atom_shape.setOrigin(radius, radius);
        atom_shape.setPosition(sf::Vector2f(atom.position));
        atom_shape.setFillColor(sf::Color((int) std::clamp(atom.getAbsoluteSpeed() * 5, 0., 255.), 0, 0));

        m_window.draw(atom_shape);
    }

    void addCallback(sf::Event::EventType event, std::function<void(const sf::Event &)> &&callback) {
        m_callbacks[event] = callback;
    }
};

#endif //PHYSICSSIMULATION_WINDOWDRAWER_H
