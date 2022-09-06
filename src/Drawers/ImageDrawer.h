#ifndef PHYSICSSIMULATION_IMAGEDRAWER_H
#define PHYSICSSIMULATION_IMAGEDRAWER_H

#include "Atom.h"
#include "Drawer.h"

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>

class ImageShape {
protected:
    sf::Vector2f m_origin;
    sf::Vector2f m_position;

    sf::Color m_fill_color;

public:
    ImageShape() = default;

    ImageShape(
            const sf::Vector2f &origin,
            const sf::Vector2f &position,
            const sf::Color &fillColor
    ) :
            m_origin(origin),
            m_position(position),
            m_fill_color(fillColor) {}

    template<class T>
    ImageShape &setOrigin(const sf::Vector2<T> &origin) {
        m_origin = origin;

        return *this;
    }

    template<class T>
    ImageShape &setOrigin(T x, T y) {
        m_origin = sf::Vector2f(x, y);

        return *this;
    }

    template<class T>
    ImageShape &setPosition(const sf::Vector2<T> &position) {
        m_position = sf::Vector2f(position);

        return *this;
    }

    template<class T>
    ImageShape &setPosition(T x, T y) {
        m_position = sf::Vector2f(x, y);

        return *this;
    }

    ImageShape &setFillColor(const sf::Color &color) {
        m_fill_color = color;

        return *this;
    }

    [[nodiscard]] virtual sf::FloatRect getBoundingBox() const = 0;

    [[nodiscard]] virtual bool isInside(int x, int y) const = 0;

    void draw(sf::Image &image) const {
        auto bb = getBoundingBox();

        auto bb_on_image = sf::FloatRect(
                std::max(0.f, bb.left),
                std::max(0.f, bb.top),
                std::min((float) image.getSize().x, bb.left + bb.width) - std::max(0.f, bb.left),
                std::min((float) image.getSize().y, bb.top + bb.height) - std::max(0.f, bb.top)
        );

        for (
                unsigned int x = std::floor(bb_on_image.left);
                x < (unsigned int) std::ceil(bb_on_image.left + bb_on_image.width);
                ++x
                ) {
            for (
                    unsigned int y = std::floor(bb_on_image.top);
                    y < (unsigned int) std::ceil(bb_on_image.top + bb_on_image.height);
                    ++y
                    ) {
                if (isInside(x - m_position.x + m_origin.x, y - m_position.y + m_origin.y))
                    image.setPixel(x, image.getSize().y - y, m_fill_color);
            }
        }
    }
};

class Rectangle : public ImageShape {
protected:
    sf::Vector2f m_size;

public:
    Rectangle() = default;

    [[nodiscard]] sf::FloatRect getBoundingBox() const override {
        return {m_position - m_origin, m_size};
    }

    [[nodiscard]] bool isInside(int x, int y) const override {
        return (float) x <= m_size.x && (float) y <= m_size.y;
    }

    template<class T>
    void setSize(T width, T height) {
        m_size = sf::Vector2f(width, height);
    }
};

class Circle : public ImageShape {
protected:
    float m_radius{0.};

public:
    Circle() = default;

    void setRadius(float radius) {
        m_radius = radius;
    }

    [[nodiscard]] sf::FloatRect getBoundingBox() const override {
        return {m_position - m_origin, sf::Vector2f(m_radius * 2.f, m_radius * 2.f)};
    }

    [[nodiscard]] bool isInside(int x, int y) const override {
        return std::pow(x - m_radius, 2) + std::pow(y - m_radius, 2) < std::pow(m_radius, 2);
    }
};

class Line : public ImageShape {
protected:
    sf::Vector2f m_size;
public:
    Line(const sf::Vector2f &first, const sf::Vector2f &second) {
        m_position = first;
        m_size = second - first;
    }

    template<class T>
    void setSize(T width, T height) {
        m_size = sf::Vector2f(width, height);
    }

    [[nodiscard]] sf::FloatRect getBoundingBox() const override {
        return {m_position, m_size};
    }

    [[nodiscard]] bool isInside(int x, int y) const override {
        return std::abs(
                (float) x / m_size.x - (float) y / m_size.y
        ) < 0.1;
    }
};

class ImageDrawer: public Drawer {
private:
    sf::Image m_image;
    sf::Vector2u m_size;

public:
    explicit ImageDrawer(const sf::Vector2u &size) : m_size(size) {}

    void startDraw() override {
        m_image.create(m_size.x, m_size.y, sf::Color::White);
    };

    void endDraw() override {
        m_image.saveToFile(path);
    }

    void drawAtom(const Atom &atom, const sf::Vector2d &box_size) override {
        float radius = 5.f;

        auto atom_pos = sf::Vector2f(
                (float) (atom.position.x / box_size.x * (float) m_size.x),
                (float) (atom.position.y / box_size.y * (float) m_size.y)
        );

        Circle atom_shape;

        atom_shape.setRadius(radius);
        atom_shape.setOrigin(radius, radius);
        atom_shape.setPosition(atom_pos);

        std::map<AtomType, sf::Color> colors = {
                {AtomType::WATER, sf::Color(212, 241, 249)},
                {AtomType::BODY, sf::Color(93, 93, 93)},
        };

        atom_shape.setFillColor(colors[atom.type]);

        atom_shape.draw(m_image);
    }

    void drawMovingWall(double moving_wall_y, const sf::Vector2d &box_size) {
        Rectangle moving_wall;

        moving_wall.setFillColor(sf::Color::Red);
        moving_wall.setSize(box_size.x, 10.);
        moving_wall.setPosition(0., moving_wall_y);

        moving_wall.draw(m_image);
    }

    void drawBorders(const sf::Vector2d &box_size) {
        Line({0, 0}, {0, (float) box_size.y})
                .setFillColor(sf::Color::Black).draw(m_image);
        Line({0, (float) box_size.y}, {(float) box_size.x, (float) box_size.y})
                .setFillColor(sf::Color::Black).draw(m_image);
        Line({(float) box_size.x, (float) box_size.y}, {(float) box_size.x, 0})
                .setFillColor(sf::Color::Black).draw(m_image);
        Line({(float) box_size.x, 0}, {0, 0})
                .setFillColor(sf::Color::Black).draw(m_image);
    }
};


#endif //PHYSICSSIMULATION_IMAGEDRAWER_H
