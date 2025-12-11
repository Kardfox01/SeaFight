#pragma once

#include <cctype>
#include <memory>
#include "../space_chief/aowindow.cxx"
#include "../space_chief/space.cxx"
#include "../space_chief/chief.cxx"

#include "wait_opponent_space.cxx"
#include "../widgets/input.cxx"


class EnterNameSpace: public Space {
    Input nameInput{"NAME: ", 6};

public:
    EnterNameSpace() {
        nameInput.setOnEnterPressed([&]() {
            Chief::global()
                .setSpace<WaitOpponentSpace>(nameInput.getContent());
        });
        nameInput.setContent("GEORGE");
    }

    void handleEvent(const std::optional<sf::Event>& event) override {
        nameInput.handleEvent(event);
    }

    void update(float dt) override {}

    void draw() override {
        auto windowSize = AOWindow::global().getSize();
        auto bounds = nameInput.getLocalBounds();
        nameInput.setPosition({
            windowSize.x / 2.f - bounds.size.x / 2.f,
            windowSize.y / 2.f - bounds.size.y / 2.f
        });
        nameInput.draw();
    }
};
