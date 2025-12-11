#pragma once

#include <iostream>
#include <cctype>
#include <sstream>
#include <vector>

#include "../space_chief/aowindow.cxx"
#include "../space_chief/space.cxx"
#include "../space_chief/chief.cxx"

#include "../AOJack.cxx"
#include "../widgets/input.cxx"
#include "../widgets/label.cxx"
#include "../widgets/fight/cell.cxx"
#include "../protocol.cxx"


class GameSpace: public Space {
    Label opponentNameLabel;

    std::string opponentName;
    jackwarp* opponentSocket = nullptr;

    std::vector<std::vector<Cell>> ownField;
    std::vector<std::vector<Cell>> opponentField;

public:
    explicit GameSpace(
        std::string opponentName,
        bool isHost
    ):
        opponentName(opponentName)
    {
        AOWindow::global().setTitle("fight");
        opponentNameLabel.setString(opponentName);

        if (isHost)
            opponentSocket = &AOJackHost::global();
        else
            opponentSocket = &AOJack::global();
        
        for (unsigned short i = 0; i < 10; ++i) {
            ownField.emplace_back(std::vector<Cell>());
            for (unsigned short j = 0; j < 10; ++j)
                ownField[i].emplace_back(
                    sf::Vector2f{i*50.f, j*50.f}
                );
        }
    }

    void handleEvent(const std::optional<sf::Event>& event) override {}

    void update(float dt) override {}

    void draw() override {
        opponentNameLabel.draw();
    }
};