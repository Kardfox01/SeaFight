#pragma once

#include <iostream>
#include <cctype>
#include <sstream>
#include <thread>

#include "../space_chief/aowindow.cxx"
#include "../space_chief/space.cxx"
#include "../space_chief/chief.cxx"

#include "../AOJack.cxx"
#include "../widgets/input.cxx"
#include "../protocol.cxx"

#include "game_space.cxx"


class WaitOpponentSpace: public Space {
    sf::Font font;
    sf::Text inviteText;
    std::unique_ptr<Input> inviteInput;
    std::string name;

    std::unique_ptr<std::thread> waitOpponentThread;

public:
    explicit WaitOpponentSpace(std::string name):
        inviteText(font),
        name(name)
    {
        font.openFromFile("courier.ttf");

        inviteText.setCharacterSize(48);
        inviteText.setFillColor(sf::Color::White);

        inviteInput = std::make_unique<Input>(
            font,
            "OR CONNECT: ",
            8,
            true,
            [this]() { connectToOpponent(); }
        );

        showInvite();
        waitOpponentThread = std::make_unique<std::thread>(
            &WaitOpponentSpace::waitOpponent,
            this
        );
        waitOpponentThread->detach();
    }

    void handleEvent(const std::optional<sf::Event>& event) override {
        inviteInput->handleEvent(event);
    }

    void update(float dt) override {
        updateTitle(dt);
    }

    void draw() override {
        AOWindow::global().draw(inviteText);
        inviteInput->draw();
    }

    ~WaitOpponentSpace() {
        auto& host = AOJackHost::global();

        if (!host.jackConnected())
            host.shutdownServer();
    }

private:
    void updateTitle(float dt) {
        static short i = -1, d = 50;
        static std::stringstream load("");

        if (i > 3) i = -1;

        if (d >= 50) {
            d = 0;
            load << "wait opponent" << std::string(++i, '.');
            AOWindow::global().setTitle(load.str());
            load.str("");
        }

        d += int(dt * 1000);
    }

    void showInvite() {
        std::stringstream inviteCode;
        inviteCode << name << ", INVITE CODE: " << AOJackHost::global().getInvite();
        inviteText.setString(inviteCode.str());

        auto windowSize = AOWindow::global().getSize();
        auto bounds = inviteText.getLocalBounds();

        inviteText.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});

        inviteText.setPosition({
            windowSize.x / 2.f,
            windowSize.y / 2.f - bounds.size.y * 2
        });
    }

    void waitOpponent() {
        try {
            auto& host = AOJackHost::global();
            host.waitJack();

            if (!host.jackConnected()) return;

            std::string opponentName;
            host >> opponentName;
            host << protocol::welcome(name);

            Chief::global()
                .setSpace<GameSpace>(opponentName);
        } catch (const JackNetworkError& error) {}
    }

    void connectToOpponent() {
        try {
            auto& client = AOJack::global(jack::inviteToIp(inviteInput->getContent()));
            client << protocol::hello(name);

            std::string opponentName;
            client >> opponentName;
            Chief::global()
                .setSpace<GameSpace>(opponentName);
        } catch (const JackError& error) {
            inviteText.setString(error.what());
        }
    }
};
