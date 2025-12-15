#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdint>
#include <stdexcept>
#include <cstdlib>
#include <vector>

#include "jackexcept.cxx"

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <Ws2tcpip.h>
    #include <iphlpapi.h>
    using socklen_t = int;
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <ifaddrs.h>
    using SOCKET = int;
    const int INVALID_SOCKET = -1;
    const int SOCKET_ERROR   = -1;
#endif


/// \brief Базовый класс для TCP-соединений.
///
/// Оборачивает низкоуровневый сокет и предоставляет базовые операции
/// ввода-вывода. Не используется напрямую.
/// Используется через классы-наследники jackhost и jack.
///
class jackwarp {
protected:
    SOCKET _host = INVALID_SOCKET; ///< Основной сокет.
    SOCKET _jack = INVALID_SOCKET; ///< Активное соединение (accept/connect).

#ifdef _WIN32
    /// \brief Инициализация WinSock (выполняется один раз).
    static void initSystem() {
        std::cout << "==> INIT WINSOCK" << std::endl;
        static bool initialized = false;
        if (!initialized) {
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
                throw JackNetworkError("WSAStartup failed");
            initialized = true;
        }
    }
    static void closeSocket(SOCKET s) noexcept { closesocket(s); }
#else
    static void initSystem() noexcept {}
    static void closeSocket(SOCKET s) noexcept { close(s); }
#endif

protected:
    /// \brief Создаёт TCP-сокет (AF_INET, SOCK_STREAM).
    ///
    /// На Windows автоматически инициализирует WinSock.
    /// \throws JackNetworkError при ошибке создания сокета.
    jackwarp(bool isHost): isHost(isHost) {
        initSystem();
        _host = socket(AF_INET, SOCK_STREAM, 0);
        if (_host == INVALID_SOCKET)
            throw JackNetworkError("socket creation failed");
    }

public:
    const bool isHost;

    bool jackConnected() {
        return _jack != INVALID_SOCKET;
    }

    bool connectedToHost() {
        return _host != INVALID_SOCKET;
    }

    void shutdownHost() noexcept {
        if (_host != INVALID_SOCKET) {
            std::cout << "==> CLOSING HOST" << std::endl;
            closeSocket(_host);
            _host = INVALID_SOCKET;
        }
    }

    void shutdownJack() noexcept {
        if (_jack != INVALID_SOCKET) {
            std::cout << "==> CLOSING JACK" << std::endl;
            closeSocket(_jack);
            _jack = INVALID_SOCKET;
        }
    }

    /// \brief Закрывает все открытые сокеты.
    virtual ~jackwarp() noexcept {
        shutdownHost();
        shutdownJack();
        #ifdef WIN32
            WSACleanup();
        #endif
    }

    jackwarp(const jackwarp&) = delete;
    jackwarp& operator=(const jackwarp&) = delete;

    //------------------------------------------------------------------
    /// \brief Отправляет строку данных в подключённый сокет.
    ///
    /// Передача выполняется полностью (цикл send до отправки всех байт).
    /// \param data Строка для отправки.
    /// \throws JackNetworkError при ошибке send или отсутствии соединения.
    jackwarp& operator<<(const std::string& data) {
        if (_jack == INVALID_SOCKET)
            throw JackNetworkError("no jack connected");

        size_t total = 0;
        while (total < data.size()) {
            int sent = send(_jack, data.data() + total, (int)(data.size() - total), 0);
            if (sent == SOCKET_ERROR || sent <= 0)
                throw JackNetworkError("send failed");
            total += static_cast<size_t>(sent);
        }

        std::cout << "==> SENDING " << data.size() << "s DATA" << std::endl;

        return *this;
    }

    //------------------------------------------------------------------
    /// \brief Читает данные из сокета в строку.
    ///
    /// Считывает максимум 1023 байта за раз.
    /// Если соединение закрыто, возвращает пустую строку.
    /// \throws JackNetworkError при ошибке recv().
    jackwarp& operator>>(std::string& out) {
        if (_jack == INVALID_SOCKET)
            throw JackNetworkError("no jack connected");

        std::cout << "==> RECV " << out.size() << "s DATA" << std::endl;

        char buffer[1024];
        int bytes = recv(_jack, buffer, static_cast<int>(sizeof(buffer) - 1), 0);
        if (bytes == SOCKET_ERROR)
            throw JackNetworkError("recv failed");

        if (bytes > 0) {
            buffer[bytes] = '\0';
            out.assign(buffer, static_cast<size_t>(bytes));
        } else {
            out.clear();
        }
        return *this;
    }
};


/// \brief Серверная сторона TCP-соединения.
///
/// Слушает указанный порт, принимает одно входящее соединение
/// и обеспечивает обмен данными через базовый интерфейс jackwarp.
///
class jackhost : public jackwarp {
    sockaddr_in host_address{};
public:
    /// \brief Создаёт серверный сокет и начинает прослушивание порта.
    ///
    /// \param port Порт для прослушивания.
    /// \throws JackNetworkError при ошибке bind() или listen().
    explicit jackhost(unsigned short port): jackwarp(true) {
        host_address.sin_family = AF_INET;
        host_address.sin_addr.s_addr = htonl(INADDR_ANY);
        host_address.sin_port = htons(port);

        if (bind(
            _host,
            reinterpret_cast<sockaddr*>(&host_address),
            static_cast<int>(sizeof(host_address))
        ) == SOCKET_ERROR)
            throw JackNetworkError("bind failed");

        if (listen(_host, SOMAXCONN) == SOCKET_ERROR)
            throw JackNetworkError("listen failed");

        std::cout << "==> INIT HOST" << std::endl;
    }

    //------------------------------------------------------------------
    /// \brief Ожидает подключение клиента.
    ///
    /// После успешного вызова _jack становится активным соединением.
    /// \throws JackNetworkError при ошибке accept().
    void waitJack() {
        sockaddr_in jack_address{};
        socklen_t jack_address_size = static_cast<socklen_t>(sizeof(jack_address));

        _jack = accept(
            _host,
            reinterpret_cast<sockaddr*>(&jack_address),
            &jack_address_size
        );

        if (_jack == INVALID_SOCKET)
            throw JackNetworkError("accept failed");

        std::cout << "==> JACK " << _jack << " CONNECTED" << std::endl;
    }

    //------------------------------------------------------------------
    /// \brief Возвращает IP-адрес текущего хоста (IPv4).
    ///
    /// \return IP в формате "x.x.x.x".
    /// \throws JackNetworkError или JackAddressError при ошибках DNS/IP.
    std::string getIp() const {
        #if defined(_WIN32)
            // Инициализация WinSock
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
                return "";

            ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
            ULONG family = AF_INET;
            ULONG size = 16 * 1024;
            std::vector<char> buffer(size);

            IP_ADAPTER_ADDRESSES* addresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
            if (GetAdaptersAddresses(family, flags, nullptr, addresses, &size) != ERROR_SUCCESS)
                return "";

            for (auto* addr = addresses; addr != nullptr; addr = addr->Next) {
                // Игнорируем неактивные и loopback интерфейсы
                if (addr->OperStatus != IfOperStatusUp)
                    continue;

                for (auto* unicast = addr->FirstUnicastAddress; unicast != nullptr; unicast = unicast->Next) {
                    SOCKADDR_IN* sa_in = reinterpret_cast<SOCKADDR_IN*>(unicast->Address.lpSockaddr);
                    char buf[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(sa_in->sin_addr), buf, sizeof(buf));

                    std::string ip = buf;
                    if (ip != "127.0.0.1")
                        return ip;
                }
            }

            return "";
        #else
            struct ifaddrs* ifaddr = nullptr;
            if (getifaddrs(&ifaddr) == -1)
                return "";

            std::string result;

            for (auto* iface = ifaddr; iface != nullptr; iface = iface->ifa_next) {
                if (!iface->ifa_addr || iface->ifa_addr->sa_family != AF_INET)
                    continue;

                // Пропускаем loopback
                if (strcmp(iface->ifa_name, "lo") == 0)
                    continue;

                char buf[INET_ADDRSTRLEN];
                void* addr = &((struct sockaddr_in*)iface->ifa_addr)->sin_addr;

                inet_ntop(AF_INET, addr, buf, sizeof(buf));
                result = buf;
                break;
            }

            freeifaddrs(ifaddr);
            return result;
        #endif
    }

    //------------------------------------------------------------------
    /// \brief Возвращает invite-код, соответствующий IP-адресу хоста.
    ///
    /// Пример: 192.168.0.5 → "c0a80005".
    /// \throws JackAddressError при ошибке преобразования.
    std::string getInvite() const {
        std::string ip = getIp();
        in_addr addr{};
        if (inet_pton(AF_INET, ip.c_str(), &addr) != 1)
            throw JackAddressError("inet_pton() failed while encoding invite");

        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&addr);
        std::ostringstream code;
        code << std::hex << std::setfill('0');
        for (int i = 0; i < 4; ++i)
            code << std::setw(2) << static_cast<int>(bytes[i]);

        return code.str();
    }
};


/// \brief Клиентская сторона TCP-соединения.
///
/// Подключается к серверу (jackhost) по IP-адресу или invite-коду.
///
class jack : public jackwarp {
    sockaddr_in host_address{};
public:
    /// \brief Создаёт подключение к указанному хосту и порту.
    ///
    /// \param host IPv4-адрес в формате "x.x.x.x".
    /// \param port Целевой порт.
    /// \throws JackAddressError при ошибке адресации.
    /// \throws JackNetworkError при ошибке подключения.
    jack(const std::string& host, unsigned short port): jackwarp(true) {
        host_address.sin_family = AF_INET;
        host_address.sin_port = htons(port);

        if (inet_pton(AF_INET, host.c_str(), &host_address.sin_addr) <= 0)
            throw JackAddressError("invalid host address");

        if (connect(_host, reinterpret_cast<sockaddr*>(&host_address),
                    static_cast<int>(sizeof(host_address))) == SOCKET_ERROR)
            throw JackNetworkError("connect failed");

        _jack = _host;
        std::cout << "==> INIT JACK" << std::endl;
    }

    //------------------------------------------------------------------
    /// \brief Преобразует invite-код в IPv4-адрес.
    ///
    /// Пример: "c0a80005" → "192.168.0.5".
    /// \throws JackInviteError при некорректном коде.
    /// \throws JackAddressError при сбое inet_ntop().
    static std::string inviteToIp(const std::string& code) {
        if (code.size() != 8)
            throw JackInviteError("invalid invite code length (expected 8 hex chars)");

        uint8_t bytes[4];
        for (int i = 0; i < 4; ++i) {
            try {
                bytes[i] = static_cast<uint8_t>(
                    std::stoi(code.substr(i * 2, 2), nullptr, 16)
                );
            } catch (const std::invalid_argument&) {
                throw JackInviteError("invite contains non-hex characters");
            } catch (const std::out_of_range&) {
                throw JackInviteError("invite contains out-of-range byte");
            }
        }

        in_addr addr{};
        std::memcpy(&addr, bytes, 4);

        char buffer[INET_ADDRSTRLEN];
        if (!inet_ntop(AF_INET, &addr, buffer, static_cast<socklen_t>(sizeof(buffer))))
            throw JackAddressError("inet_ntop() failed while decoding invite");

        return buffer;
    }
};
