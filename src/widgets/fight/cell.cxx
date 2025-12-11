#pragma once

#include <SFML/Graphics.hpp>
#include "../../space_chief/aowindow.cxx"


class Cell {
    sf::RectangleShape cell;

public:
    Cell(
        sf::Vector2f position,
        float size = 50.f
    ) {
        cell.setSize({size, size});
        cell.setPosition(position);

        cell.setFillColor(sf::Color::Black);
        cell.setOutlineThickness(5);
        cell.setOutlineColor(sf::Color::White);
    }

    void handleEvent(const std::optional<sf::Event>& event) {}

    void draw() const {
        AOWindow::global().draw(cell);
    }

    ~Cell() = default;
};