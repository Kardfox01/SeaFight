#pragma once

#include "aowindow.cxx"
#include "space.cxx"


class Chief {
    Chief() = default;
    Chief(const Chief&) = delete;
    Chief& operator=(const Chief&) = delete;

    std::unique_ptr<Space> current;

public:
    static Chief& global() {
        static Chief instance;
        return instance;
    }

    template<class T, class... Args>
    Chief& setSpace(Args&&... args) {
        current = std::make_unique<T>(std::forward<Args>(args)...);
        return *this;
    }

    void run() {
        auto& window = AOWindow::global();
        sf::Clock clock;

        while (window.isOpen() && current) {
            float dt = clock.restart().asSeconds();

            while (const auto event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                    break;
                }

                current->handleEvent(event);
            }

            current->update(dt);

            window.clear();
            current->draw();
            window.display();
        }
    }
};