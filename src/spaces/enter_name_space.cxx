#pragma once

#include <cctype>
#include <memory>
#include "../space_chief/aowindow.cxx"
#include "../space_chief/space.cxx"
#include "../space_chief/chief.cxx"

#include "wait_opponent_space.cxx"
#include "../widgets/input.cxx"


class EnterNameSpace: public Space {
    sf::Font font;
    std::optional<Input> nameInput;
    std::string name;

public:
    EnterNameSpace() {
        font.openFromFile("courier.ttf");

        nameInput.emplace(font, "NAME: ", 6, true, [&]() {
            Chief::global()
                .setSpace<WaitOpponentSpace>(nameInput->getContent());
        });
    }

    void handleEvent(const std::optional<sf::Event>& event) override {
        nameInput->handleEvent(event);
    }

    void update(float dt) override {}

    void draw() override {
        nameInput->draw();
    }
};
