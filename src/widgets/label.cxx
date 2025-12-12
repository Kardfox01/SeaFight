#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "../space_chief/aowindow.cxx"


class Label {
    sf::Font font;
    sf::Text text;
    bool centered = false;

public:
    Label(
        unsigned short characterSize = 48
    ):
        text(font)
    {
        font.openFromFile("courier.ttf");
        text.setFont(font);
        text.setCharacterSize(characterSize);
        text.setFillColor(sf::Color::White);
    }

    void setPosition(const sf::Vector2f& position) {
        text.setPosition(position);
    }

    void setString(const std::string& newContent) {
        text.setString(newContent);
        if (centered)
            text.setOrigin({
                text.getLocalBounds().size.x / 2,
                0
            });
    }

    void isCentered(bool flag) {
        centered = flag;
    }

    void draw() const {
        AOWindow::global().draw(text);
    }

    sf::FloatRect getLocalBounds() const {
        return text.getLocalBounds();
    }
};
