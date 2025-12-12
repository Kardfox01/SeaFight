#pragma once

#include <SFML/Graphics.hpp>
#include "../../space_chief/aowindow.cxx"


class Cell {
    sf::RectangleShape cell;
    sf::Color firstColor;
    bool placed = false;
    bool shoted = false;
    bool destroyed = false;

public:
    static constexpr float SIZE = 60.f;

    Cell(
        sf::Vector2f position,
        sf::Color color = sf::Color::Black
    ): firstColor(color) {
        cell.setSize({Cell::SIZE, Cell::SIZE});
        cell.setPosition(position);

        cell.setFillColor(sf::Color::Black);
        cell.setOutlineThickness(2);
        cell.setOutlineColor(sf::Color(170, 170, 170));
    }

    void setColor(sf::Color newColor) {
        cell.setFillColor(newColor);
    }

    void handleEvent(const std::optional<sf::Event>& event) {}

    void place() {
        placed = true;
        shoted = false;
        cell.setOutlineThickness(0);
        cell.setFillColor(firstColor);
    }

    void shot() {
        shoted = true;
        cell.setFillColor(sf::Color(0, 170, 100));
    }

    void destroy() {
        destroyed = true;
        cell.setFillColor(sf::Color::Magenta);
    }

    void reset() {
        shoted = false;
        placed = false;
        cell.setOutlineThickness(2);
        cell.setFillColor(sf::Color::Black);
    }

    void draw() const {
        AOWindow::global().draw(cell);
    }

    float x() {
        return cell.getPosition().x;
    }

    float y() {
        return cell.getPosition().y;
    }

    bool isPlaced() {
        return placed;
    }

    bool isShoted() {
        return shoted;
    }

    bool isDestroyed() {
        return destroyed;
    }

    ~Cell() = default;
};