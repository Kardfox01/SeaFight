#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include "../space_chief/aowindow.cxx"

class Input {
    sf::Font& font;
    sf::Text inputText;
    std::string content;
    std::string prompt;
    size_t maxLength;
    bool centered;
    std::function<void()> onEnterPressed;

public:
    Input(
        sf::Font& font,
        const std::string& prompt,
        size_t maxLength = 5,
        bool centered = false,
        std::function<void()> onEnterPressed = nullptr
    ):
        font(font),
        inputText(font),
        prompt(prompt),
        maxLength(maxLength),
        centered(centered),
        onEnterPressed(onEnterPressed)
    {
        inputText.setFont(font);
        inputText.setCharacterSize(48);
        inputText.setFillColor(sf::Color::White);
        updateText();

        if (centered) {
            centerText();
        }
    }

    void handleEvent(const std::optional<sf::Event>& event) {
        if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            if (key->scancode == sf::Keyboard::Scancode::Backspace && !content.empty()) {
                content.pop_back();
                updateText();
            } else if (key->scancode == sf::Keyboard::Scancode::Enter && !content.empty()) {
                if (onEnterPressed) onEnterPressed();
            } else {
                if (content.size() >= maxLength) return;
                auto keycode = key->scancode;
                if (
                    keycode >= sf::Keyboard::Scancode::A && keycode <= sf::Keyboard::Scancode::Z ||
                    keycode >= sf::Keyboard::Scancode::Num1 && keycode <= sf::Keyboard::Scancode::Num0
                ) {
                    content.push_back(sf::Keyboard::getDescription(keycode).toAnsiString()[0]);
                    updateText();
                }
            }
        }
    }

    void updateText() {
        if (content.empty())
            inputText.setString(prompt + "-");
        else if (content.size() < maxLength)
            inputText.setString(prompt + content + "-");
        else
            inputText.setString(prompt + content);

        if (centered) {
            centerText();
        }
    }

    void setPosition(const sf::Vector2f& position) {
        inputText.setPosition(position);
    }

    void setOrigin(const sf::Vector2f& origin) {
        inputText.setOrigin(origin);
    }

    void draw() const {
        AOWindow::global().draw(inputText);
    }

    const std::string& getContent() const {
        return content;
    }

    bool isEmpty() const {
        return content.empty();
    }

    sf::FloatRect getLocalBounds() const {
        return inputText.getLocalBounds();
    }

    void setOnEnterPressed(std::function<void()> callback) {
        onEnterPressed = callback;
    }

private:
    void centerText() {
        auto windowSize = AOWindow::global().getSize();
        auto bounds = inputText.getLocalBounds();
        inputText.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.0f});
        inputText.setPosition({
            windowSize.x / 2.f,
            windowSize.y / 2.f
        });
    }
};