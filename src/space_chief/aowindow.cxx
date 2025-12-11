#pragma once

#include <SFML/Graphics.hpp>

using window_t = sf::RenderWindow;

class AOWindow {
    AOWindow() = default;
    AOWindow(const AOWindow&) = delete;
    AOWindow& operator=(const AOWindow&) = delete;

public:
    static window_t& global(
        const std::string& title = "not implemented",
        const sf::Vector2u size  = {1500, 900}
    ) {
        static auto instance = std::make_unique<window_t>(
            sf::VideoMode(size),
            title,
            sf::Style::Titlebar | sf::Style::Close
        );
        return *instance;
    }
};