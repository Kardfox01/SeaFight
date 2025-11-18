#pragma  once

#include <string>


namespace protocol {

//
// === Базовые служебные команды ===
//

inline std::string hello(const std::string& name) noexcept {
    return "HELLO " + name + "\n";
}

inline std::string welcome(const std::string& name) noexcept {
    return "WELCOME " + name + "\n";
}

inline std::string ready() noexcept {
    return "READY\n";
}

inline std::string start() noexcept {
    return "START\n";
}

inline std::string bye() noexcept {
    return "BYE\n";
}

inline std::string error(const std::string& message) noexcept {
    return "ERROR " + message + "\n";
}

//
// === Игровой процесс ===
//

inline std::string shot(const std::string& cell) noexcept {
    return "SHOT " + cell + "\n";
}

inline std::string result(const std::string& cell, const std::string& status) noexcept {
    return "RESULT " + cell + " " + status + "\n";
}

inline std::string turn() noexcept {
    return "TURN\n";
}

inline std::string winner(const std::string& name) noexcept {
    return "WINNER " + name + "\n";
}

inline std::string restart() noexcept {
    return "RESTART\n";
}

inline std::string accept() noexcept {
    return "ACCEPT\n";
}

inline std::string decline() noexcept {
    return "DECLINE\n";
}

//
// === Дополнительные команды ===
//

inline std::string ping() noexcept {
    return "PING\n";
}

inline std::string pong() noexcept {
    return "PONG\n";
}

}