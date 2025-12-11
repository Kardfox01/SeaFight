#pragma once

#include "jack/jack.cxx"
#include <memory>

class AOJack {
    AOJack() = default;
    AOJack(const AOJack&) = delete;
    AOJack& operator=(const AOJack&) = delete;

public:
    static jack& global(
        const std::string& host = "",
        unsigned short port     = 5555
    ) {
        static auto instance = std::make_unique<jack>(host, port);
        if (instance)
            return *instance;
    }
};

class AOJackHost {
    AOJackHost() = default;
    AOJackHost(const AOJackHost&) = delete;
    AOJackHost& operator=(const AOJackHost&) = delete;

public:
    static jackhost& global(
        unsigned short port = 5555
    ) {
        static auto instance = std::make_unique<jackhost>(port);
        if (instance)
            return *instance;
    }
};
