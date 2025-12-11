#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include "../space_chief/aowindow.cxx"


class Input {
    sf::Font font;
    sf::Text inputText;
    std::string content;
    std::string prompt;
    size_t maxLength;
    std::function<void()> onEnterPressed = nullptr;

public:
    Input(
        const std::string& prompt,
        size_t maxLength = 5
    ):
        inputText(font),
        prompt(prompt),
        maxLength(maxLength)
    {
        font.openFromFile("courier.ttf");
        inputText.setFont(font);
        inputText.setCharacterSize(48);
        inputText.setFillColor(sf::Color::White);
        updateText();
    }

    void handleEvent(const std::optional<sf::Event>& event) {
        if (
            auto* key = event->getIf<sf::Event::TextEntered>()
        ) {
            switch (key->unicode) {
                case 8: // Backspace
                    if (content.empty()) return;
                    content.pop_back();
                    updateText();
                    break;
                case 13: // Enter
                    if (
                        onEnterPressed && !content.empty()
                    )
                        onEnterPressed();
                    break;
                default:
                    if (
                        content.size() == maxLength ||
                        !std::isalnum(key->unicode)
                    ) break;
                    content.push_back(static_cast<char>(key->unicode));
                    updateText();
            }
        }
    }

    void updateText() {
        if (content.size() <= maxLength)
            inputText.setString(
                prompt + content + std::string(maxLength - content.size(), '-')
            );
    }

    void setPosition(const sf::Vector2f& position) {
        inputText.setPosition(position);
    }

    void draw() const {
        AOWindow::global().draw(inputText);
    }

    const std::string& getContent() const {
        return content;
    }

    void setContent(std::string&& newContent) {
        content = newContent;
        updateText();
    }

    void clear() {
        content = "";
        updateText();
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

    ~Input() = default;
};