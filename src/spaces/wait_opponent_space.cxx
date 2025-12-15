#pragma once

#include <iostream>
#include <cctype>
#include <sstream>
#include <thread>

#include "../space_chief/aowindow.cxx"
#include "../space_chief/space.cxx"
#include "../space_chief/chief.cxx"

#include "../aojack.cxx"
#include "../widgets/input.cxx"
#include "../widgets/label.cxx"
#include "../protocol.cxx"

#include "place_space.cxx"


class WaitOpponentSpace: public Space {
    Label inviteText;
    Input inviteInput{"OR CONNECT: ", 8};
    std::string name;
    bool wasError = false;

    std::unique_ptr<std::thread> waitOpponentThread;

public:
    explicit WaitOpponentSpace(std::string name): name(name) {
        inviteInput.setOnEnterPressed([&] {
            AOWindow::global().setTitle("connecting...");
            connectToOpponent();
        });
        showInvite();
        waitOpponentThread = std::make_unique<std::thread>(
            &WaitOpponentSpace::waitOpponent,
            this
        );
        waitOpponentThread->detach();

        inviteInput.setContent("c0a80014");
    }

    void handleEvent(const std::optional<sf::Event>& event) override {
        inviteInput.handleEvent(event);
    }

    void update(float dt) override {
        updateTitle(dt);
        resetError(dt);
    }

    void draw() override {
        auto windowSize = AOWindow::global().getSize();
        auto inviteBounds = inviteText.getLocalBounds();
        auto inputBounds  = inviteInput.getLocalBounds();

        inviteText.setPosition({
            windowSize.x / 2.f - inviteBounds.size.x / 2.f,
            windowSize.y / 2.f - inviteBounds.size.y / 2.f - inputBounds.size.y - 20
        });

        inviteInput.setPosition({
            windowSize.x / 2.f - inputBounds.size.x / 2.f,
            windowSize.y / 2.f - inputBounds.size.y / 2.f + 20
        });

        inviteText.draw();
        inviteInput.draw();
    }

    ~WaitOpponentSpace() {
        auto& host = AOJackHost::global();

        if (!host.jackConnected())
            host.shutdownHost();
    }

private:
    void resetError(float dt) {
        static short d = 0;

        if (d >= 6000) {
            showInvite();
            d = 0;
            wasError = false;
        }

        if (wasError)
            d += 1;
    }

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
        try {
            inviteCode << name << ", INVITE CODE: " << AOJackHost::global().getInvite();
            inviteText.setString(inviteCode.str());
        } catch (const JackAddressError& _) {
            wasError = true;
            inviteText.setString("LOST INTERNET");
        }
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
                .setSpace<PlaceSpace<jackhost>>(host, name, opponentName);
        } catch (const JackError& error) {
            wasError = true;
            inviteText.setString(error.what());
        }

        std::cout << "=>= EXIT FROM WAIT OPPONENT THREAD" << std::endl;
    }

    void connectToOpponent() {
        try {
            auto& client = AOJack::global(
                jack::inviteToIp(inviteInput.getContent())
            );
            client << protocol::hello(name);

            std::string opponentName;
            client >> opponentName;
            Chief::global()
                .setSpace<PlaceSpace<jack>>(client, name, opponentName);
        } catch (const JackError& error) {
            inviteText.setString(error.what());
            inviteInput.clear();
            wasError = true;
        }
    }
};
