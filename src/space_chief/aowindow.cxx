#pragma once

#include <SFML/Graphics.hpp>

using window_t = sf::RenderWindow;

class AOWindow {
    AOWindow() = default;
    AOWindow(const AOWindow&) = delete;
    AOWindow& operator=(const AOWindow&) = delete;

public:
    static window_t& global(
        const sf::Vector2u size = sf::Vector2u({800, 600}),
        const std::string& title = "not implemented"
    ) {
        static auto instance = std::make_unique<window_t>(sf::VideoMode(size), title);
        return *instance;
    }
};