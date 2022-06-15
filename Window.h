#ifndef PHYSICSSIMULATION_WINDOW_H
#define PHYSICSSIMULATION_WINDOW_H

#include "Atom.h"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <map>
#include <functional>
#include <vector>

class Window {
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
    Window(unsigned int width, unsigned int height)
            : m_window({width, height}, "Particles"),
              m_view() {
        m_view.setSize((float) width, (float) height);
        m_view.setCenter(0, 0);
        m_view.setRotation(180);
        m_view.zoom(2);

        m_window.setView(m_view);

        m_font.loadFromFile("fonts/arial.ttf");
    };

    sf::Vector2d getClickCoordinate() const {
        auto click_pos = sf::Mouse::getPosition(m_window);

        return (sf::Vector2d) m_window.mapPixelToCoords(click_pos, m_view);
    }

    [[nodiscard]] bool isOpen() const {
        return m_window.isOpen();
    }

    void startDraw() {
        m_window.clear(sf::Color::White);
        m_dt = m_dt_clock.restart();
    };

    void endDraw() {
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

    void drawAtom(const Atom &atom, float radius = 5.f) {
        sf::CircleShape atom_shape;

        atom_shape.setRadius(radius);
        atom_shape.setOrigin(radius, radius);
        atom_shape.setPosition(sf::Vector2f(atom.position));
        atom_shape.setFillColor(sf::Color((int) std::clamp(atom.getAbsoluteSpeed() * 5, 0., 255.), 0, 0));

        m_window.draw(atom_shape);
    }

    void drawSpeedDistribution(std::vector<double> &speed, int points = 50) {
        sf::Vector2f coordinate_center{100.f, 1000.f};

        std::sort(speed.begin(), speed.end());

        double min = speed.front();
        double max = speed.back();

        double tick_size = (max - min) / points;
        auto count_in_tick = new int[points];

        int j = 0;

        for (int i = 0; i < points; ++i) {
            count_in_tick[i] = 0;

            while (speed[j] < (i + 1) * tick_size && j < speed.size()) {
                ++j;
                ++count_in_tick[i];
            }
        }

        m_window.setView(m_window.getDefaultView());

        sf::VertexArray coordinateAxes(sf::Lines, 4);

        coordinateAxes[0] = sf::Vertex({coordinate_center.x, 0}, sf::Color::Black);
        coordinateAxes[1] = sf::Vertex({coordinate_center.x, coordinate_center.y * 2}, sf::Color::Black);

        coordinateAxes[2] = sf::Vertex({0, coordinate_center.y}, sf::Color::Black);
        coordinateAxes[3] = sf::Vertex({1100, coordinate_center.y}, sf::Color::Black);

        m_window.draw(coordinateAxes);


        sf::VertexArray lineChart(sf::Lines, 2 * points + 2);

        lineChart[0] = sf::Vertex(
                sf::Vector2f(coordinate_center.x, coordinate_center.y - (float) count_in_tick[0] * 10),
                sf::Color::Black
        );

        for (int i = 0; i < points; ++i) {
            lineChart[2 * i + 1] = sf::Vertex(
                    sf::Vector2f(coordinate_center.x + (float) (i * 1000. / points),
                                 coordinate_center.y - (float) count_in_tick[i] * 10),
                    sf::Color::Black
            );

            lineChart[2 * i + 2] = sf::Vertex(
                    sf::Vector2f(coordinate_center.x + (float) (i * 1000. / points),
                                 coordinate_center.y - (float) count_in_tick[i] * 10),
                    sf::Color::Black
            );
        }

        lineChart[2 * points + 1] = sf::Vertex(
                sf::Vector2f(coordinate_center.x + 1000.f,
                             coordinate_center.y - (float) count_in_tick[points - 1] * 10),
                sf::Color::Black
        );

        m_window.draw(lineChart);

        delete[] count_in_tick;
    }

    void drawMovingWall(double moving_wall_y, const sf::Vector2d &box_size) {
        sf::RectangleShape moving_wall;

        moving_wall.setFillColor(sf::Color::Red);
        moving_wall.setSize({(float) box_size.x, 10.f});
        moving_wall.setPosition(0, (float) moving_wall_y);

        m_window.draw(moving_wall);
    }

    void drawBorders(const sf::Vector2d &box_size) {
        sf::VertexArray borders(sf::LineStrip, 5);

        borders[0] = sf::Vertex({0, 0}, sf::Color::Red);
        borders[1] = sf::Vertex({0, (float) box_size.y}, sf::Color::Red);
        borders[2] = sf::Vertex({(float) box_size.x, (float) box_size.y}, sf::Color::Red);
        borders[3] = sf::Vertex({(float) box_size.x, 0}, sf::Color::Red);
        borders[4] = borders[0];

        m_window.draw(borders);
    }

    void drawCoordinateAxes() {
        sf::VertexArray coordinateAxes(sf::Lines, 4);

        coordinateAxes[0] = sf::Vertex({-1000, 0}, sf::Color::Black);
        coordinateAxes[1] = sf::Vertex({1000, 0}, sf::Color::Black);

        coordinateAxes[2] = sf::Vertex({0, -1000}, sf::Color::Black);
        coordinateAxes[3] = sf::Vertex({0, 1000}, sf::Color::Black);

        m_window.draw(coordinateAxes);
    }

    void addCallback(sf::Event::EventType event, std::function<void(const sf::Event &)> &&callback) {
        m_callbacks[event] = callback;
    }

    void drawDebugInfo(std::vector<std::pair<std::string, std::string>> &&info) {
        m_window.setView(m_window.getDefaultView());

        sf::Vector2f start_position{10.f, 10.f};
        float offset = 10.f;
        unsigned int font_size = 35;

        for (auto &[title, value]: info) {
            sf::Text text;

            title.append(": ");
            title.append(value);

            text.setString(title);

            text.setCharacterSize(font_size);
            text.setFillColor(sf::Color::Black);
            text.setFont(m_font);

            text.setPosition(start_position);

            start_position.y += offset + (float) font_size;

            m_window.draw(text);
        }

        m_window.setView(m_view);
    }
};

#endif //PHYSICSSIMULATION_WINDOW_H
