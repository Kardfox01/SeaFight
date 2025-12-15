#pragma once

#include <iostream>
#include <cctype>
#include <sstream>
#include <vector>
#include <thread>
#include <memory>

#include "../space_chief/aowindow.cxx"
#include "../space_chief/space.cxx"
#include "../space_chief/chief.cxx"

#include "../aojack.cxx"
#include "../widgets/label.cxx"
#include "../widgets/fight/fields.cxx"
#include "../protocol.cxx"


template<class jacktype>
class GameSpace: public Space {
    Label opponentNameLabel, nameLabel;
    std::vector<sf::Vertex> line;

    Field field;
    OpponentField opponentField;

    std::string opponentName;

    jacktype& opponentJack;
    std::thread listenThread;

    bool wasError = true;

public:
    explicit GameSpace(
        jacktype& opponentJack,
        std::string opponentName,
        Field field
    ):
        opponentJack(opponentJack),
        opponentName(opponentName),
        field(field)
    {
        AOWindow::global().setTitle("fight");

        nameLabel.isCentered(true);
        nameLabel.setString(opponentJack.isHost? "MOVE" : "");

        opponentNameLabel.isCentered(true);
        opponentNameLabel.setString(opponentName);

        auto windowSize = AOWindow::global().getSize();

        line = {
            sf::Vertex { sf::Vector2f(windowSize.x / 2, 0) },
            sf::Vertex { sf::Vector2f(windowSize.x / 2, windowSize.y) }
        };

        nameLabel.setPosition({
            windowSize.x / 4,
            25.f
        });
        opponentNameLabel.setPosition({
            windowSize.x - windowSize.x / 4,
            25.f
        });

        auto xPadding = (windowSize.x / 2 - 10*Cell::SIZE) / 2;
        auto yPadding = nameLabel.getLocalBounds().size.y + 75.f;

        opponentField = OpponentField(windowSize.x, xPadding, yPadding);

        listenThread = std::thread(GameSpace::listen, this);
    }

    void handleEvent(const std::optional<sf::Event>& event) override {}

    void update(float dt) override {}

    void listen() {
        bool running = true;
        std::string message;

        while (running) {
            opponentJack >> message;
            std::cout << message << std::endl;
        }
    }

    void draw() override {
        field.draw();
        opponentField.draw();
        nameLabel.draw();
        opponentNameLabel.draw();
        AOWindow::global().draw(line.data(), line.size(), sf::PrimitiveType::Lines);
    }
};