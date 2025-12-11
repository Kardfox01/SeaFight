#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "../space_chief/aowindow.cxx"


class Label {
    sf::Font font;
    sf::Text text;

public:
    Label():
        text(font)
    {
        font.openFromFile("courier.ttf");
        text.setFont(font);
        text.setCharacterSize(48);
        text.setFillColor(sf::Color::White);
    }

    void setPosition(const sf::Vector2f& position) {
        text.setPosition(position);
    }

    void setString(const std::string& newContent) {
        text.setString(newContent);
    }

    void draw() const {
        AOWindow::global().draw(text);
    }

    sf::FloatRect getLocalBounds() const {
        return text.getLocalBounds();
    }
};
