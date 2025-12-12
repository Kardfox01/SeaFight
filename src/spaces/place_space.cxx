#include "../space_chief/space.cxx"
#include "../widgets/label.cxx"
#include "../widgets/fight/fields.cxx"
#include "game_space.cxx"


class PlaceSpace: public Space {
    std::string name;
    std::string opponentName;
    bool isHost;

    Label nameLabel;
    Label opponentNameLabel;
    Field field;

    jackwarp* opponentJack;

    bool wasMistake = true;

public:
    explicit PlaceSpace(
        std::string name,
        std::string opponentName,
        bool isHost
    ):
        name(name),
        opponentName(opponentName),
        isHost(isHost)
    {
        AOWindow::global().setTitle("place");
        nameLabel.isCentered(true);
        nameLabel.setString(name);

        opponentNameLabel.isCentered(true);
        opponentNameLabel.setString(opponentName);

        auto windowSize = AOWindow::global().getSize();

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
        field = Field(windowSize.x, xPadding, yPadding);
    }

    void place() {
        sf::Vector2i mouse = sf::Mouse::getPosition(AOWindow::global());

        auto fieldFirst = field.firstPosition();
        auto fieldLast = field.lastPosition();

        static sf::Vector2i begin{-1, -1};

        if (
            (mouse.x >= fieldFirst.x && mouse.x <= fieldLast.x + Cell::SIZE) &&
            (mouse.y >= fieldFirst.y && mouse.y <= fieldLast.y + Cell::SIZE)
        ) {
            auto y = short((mouse.x - fieldFirst.x) / Cell::SIZE);
            auto x = short((mouse.y - fieldFirst.y) / Cell::SIZE);

            if (begin.x == -1) {
                if (field.placeFirst(x, y))
                    begin = { x, y };
            } else {
                if (begin.x == x && begin.y == y) {
                    field.get(x, y).reset();
                    begin = {-1, -1};
                    return;
                }

                switch (field.place(begin, {x, y})) {
                    case Placed::YES:
                        field.get(begin.x, begin.y).place();
                        begin = {-1, -1};
                        break;
                    case Placed::ISNEAR:
                        wasMistake = true;
                        nameLabel.setString("OTHER IS NEAR");
                        break;
                    case Placed::DIAGONAL:
                        wasMistake = true;
                        nameLabel.setString("PLACE BY AXIS");
                        break;
                    case Placed::OUTOF:
                        wasMistake = true;
                        nameLabel.setString("TOO LONG");
                        break;
                    case Placed::ENOUGH:
                        wasMistake = true;
                        nameLabel.setString("IT'S ENOUGH");
                        break;
                }
            }

            if (!field.checkRemaind())
                Chief::global()
                    .setSpace<GameSpace>(opponentJack, opponentName, field, isHost);
        }
    }

    void handleEvent(const std::optional<sf::Event>& event) override {
        if (const auto* _ = event->getIf<sf::Event::MouseButtonPressed>())
            place();
    }

    void update(float dt) override {
        static short d = 0;

        if (d >= 4000) {
            nameLabel.setString("PLACE");
            d = 0;
            wasMistake = false;
        }

        if (wasMistake)
            d += 1;
    }

    void draw() override {
        field.draw();
        nameLabel.draw();
    }
};
