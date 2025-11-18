#pragma once

#include <iostream>
#include <cctype>
#include <sstream>

#include "../space_chief/aowindow.cxx"
#include "../space_chief/space.cxx"
#include "../space_chief/chief.cxx"

#include "../AOJack.cxx"
#include "../widgets/input.cxx"
#include "../protocol.cxx"


class GameSpace: public Space {
    sf::Font font;
    sf::Text opponentNameText;

    std::string opponentName;

public:
    explicit GameSpace(
        std::string opponentName
    ):
        opponentNameText(font),
        opponentName(opponentName)
    {
        font.openFromFile("courier.ttf");
        AOWindow::global().setTitle("fight");

        opponentNameText.setCharacterSize(48);
        opponentNameText.setFillColor(sf::Color::White);
        opponentNameText.setString(opponentName);
    }

     void handleEvent(const std::optional<sf::Event>& event) override {}

    void update(float dt) override {}

    void draw() override {
        AOWindow::global().draw(opponentNameText);
    }
};