#pragma once

#include <SFML/Graphics.hpp>
#include <optional>


class Space {
public:
    virtual ~Space() = default;
    virtual void handleEvent(const std::optional<sf::Event>& event) = 0;
    virtual void update(float dt) = 0;
    virtual void draw() = 0;
};
