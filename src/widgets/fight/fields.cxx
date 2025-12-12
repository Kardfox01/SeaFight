#pragma once

#include <cmath>

#include "../../space_chief/aowindow.cxx"
#include "cell.cxx"


enum Placed {
    YES,
    DIAGONAL,
    ISNEAR,
    OUTOF,
    ENOUGH
};


class Field {
    std::vector<std::vector<Cell>> field{10, std::vector<Cell>()};
    int counts[4] = { 0, 8, 9, 8};

public:
    Field() = default;

    Field(
        float windowWidth,
        float xPadding,
        float yPadding
    ) {
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 10; ++j)
                field[i].emplace_back(
                    sf::Vector2f{
                        xPadding + i*Cell::SIZE,
                        yPadding + j*Cell::SIZE
                    },
                    sf::Color{0, 150 + 3*i, 150 + 5*j + 5*i}
                );
        }
    }

    bool checkNears(int x, int y) {
        for (int i = std::max(0, y - 1); i <= std::min(y + 1, 9); ++i)
            for (int j = std::max(0, x - 1); j <= std::min(x + 1, 9); ++j)
                if (field[i][j].isPlaced()) return true;

        return false;
    }

    bool checkRemaind() {
        int count = 0;
        for (int i = 1; i < 4; ++i)
            count += counts[i];

        return count > 0;
    }

    Placed place(sf::Vector2i begin, sf::Vector2i end) {
        sf::Vector2i minPoint, maxPoint;
        minPoint.x = std::min(begin.x, end.x);
        minPoint.y = std::min(begin.y, end.y);
        maxPoint.x = std::max(begin.x, end.x);
        maxPoint.y = std::max(begin.y, end.y);

        std::vector<sf::Vector2i> points;

        if (minPoint.x == maxPoint.x) {
            int size = maxPoint.y - minPoint.y + 1;
            if (size > 4) return Placed::OUTOF;
            if (counts[size - 1] == 0) return Placed::ENOUGH;

            for (int y = minPoint.y; y <= maxPoint.y; ++y) {
                if (checkNears(minPoint.x, y)) return Placed::ISNEAR;
                points.emplace_back(minPoint.x, y);
            }

            counts[size - 1] -= size;
        }

        else if (minPoint.y == maxPoint.y) {
            int size = maxPoint.x - minPoint.x + 1;
            if (size > 4) return Placed::OUTOF;
            if (counts[size - 1] == 0) return Placed::ENOUGH;

            for (int x = minPoint.x; x <= maxPoint.x; ++x) {
                if (checkNears(x, minPoint.y)) return Placed::ISNEAR;
                points.emplace_back(x, minPoint.y);
            }

            counts[size - 1] -= size;
        }

        else {
            return Placed::DIAGONAL;
        }

        for (auto& point : points)
            field[point.y][point.x].place();

        return Placed::YES;
    }

    bool placeFirst(int x, int y) {
        if (!checkNears(x, y))
            field[y][x].setColor(sf::Color::White);
        else
            return false;

        return true;
    }

    Cell& get(int x, int y) {
        return field[y][x];
    }

    sf::Vector2f firstPosition() {
        return {field[0][0].x(), field[0][0].y()};
    }

    sf::Vector2f lastPosition() {
        return {field[9][9].x(), field[9][9].y()};
    }

    void draw() {
        for (int i = 0; i < 10; ++i)
            for (int j = 0; j < 10; ++j)
                field[i][j].draw();
    }
};


class OpponentField {
    std::vector<std::vector<Cell>> field;

public:
    OpponentField() = default;

    OpponentField(
        float windowWidth,
        float xPadding,
        float yPadding
    ) {
        for (int i = 0; i < 10; ++i) {
            field.emplace_back();
            for (int j = 0; j < 10; ++j)
                field[i].emplace_back(
                    sf::Vector2f{
                        windowWidth / 2 + i*Cell::SIZE + xPadding,
                        yPadding + j*Cell::SIZE
                    },
                    sf::Color{40 + 5*j + 5*i, 0, 40 + 3*j + 3*i}
                );
        }
    }

    void onClick(int i, int j) {
        field[i][j].shot();
    }

    sf::Vector2f firstPosition() {
        return { field[0][0].x(), field[0][0].y() };
    }

    sf::Vector2f lastCellPos() {
        return { field[9][9].x(), field[9][9].y() };
    }

    void draw() {
        for (int i = 0; i < 10; ++i)
            for (int j = 0; j < 10; ++j)
                field[i][j].draw();
    }
};
