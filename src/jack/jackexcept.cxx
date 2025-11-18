#pragma once
#include <stdexcept>
#include <string>

// Базовый класс для всех ошибок jack
class JackError : public std::runtime_error {
public:
    explicit JackError(const std::string& msg)
        : std::runtime_error(msg) {}
};

// Ошибки сетевых операций (socket, bind, listen, send, recv)
class JackNetworkError : public JackError {
public:
    explicit JackNetworkError(const std::string& msg)
        : JackError(msg) {}
};

// Ошибки адресации и преобразования IP
class JackAddressError : public JackError {
public:
    explicit JackAddressError(const std::string& msg)
        : JackError(msg) {}
};

// Ошибки логики invite-кодов
class JackInviteError : public JackError {
public:
    explicit JackInviteError(const std::string& msg)
        : JackError(msg) {}
};
