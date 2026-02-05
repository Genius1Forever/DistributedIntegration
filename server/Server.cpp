#include "Server.h"
#include "../common/Logger.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

/**
 * @brief Конструктор сервера
 *
 * @param port Порт для прослушивания подключений
 */

Server::Server(int port) : port_(port) {
}

/**
 * @brief Запуск сервера
 *
 * Сервер принимает подключения клиентов,
 * получает количество ядер,
 * суммирует результаты интегрирования.
 */

void Server::start() {

    Logger::log("Starting server...");

#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&addr, sizeof(addr));
    listen(serverSocket, 5);

    Logger::log("Server listening on port " + std::to_string(port_));

    // Ввод параметров интеграла

    double a, b, h;
    int method = 1;

    std::cout << "Выберите метод интегрирования:\n";
    std::cout << "1 - Метод трапеций\n";
    std::cout << "2 - Метод Симпсона\n";
    std::cout << "> ";
    std::cin >> method;

    std::cout << "Введите a: ";
    std::cin >> a;

    std::cout << "Введите b: ";
    std::cin >> b;

    std::cout << "Введите h: ";
    std::cin >> h;

    struct ClientInfo {
        int socket;
        int cores;
    };

    std::vector<ClientInfo> clients;

    Logger::log("Ожидание клиентов...");

    // Приём клиентов

    const int MAX_CLIENTS = 2;

    // Ожидание подключения клиентов

    while ((int)clients.size() < MAX_CLIENTS) {

        int client = accept(serverSocket, nullptr, nullptr);

        Logger::log("Подключён клиент");

        char buffer[128]{};
        recv(client, buffer, sizeof(buffer), 0);

        int cores = 1;

        std::stringstream ss(buffer);
        std::string cmd;

        ss >> cmd >> cores;

        Logger::log("Клиент ядер: " + std::to_string(cores));

        clients.push_back({ client, cores });
    }

    // Деление диапазона пропорционально ядрам

    double totalLength = b - a;

    int totalCores = 0;

    for (auto& c : clients)
        totalCores += c.cores;

    double step = totalLength / totalCores;

    double current = a;

    // Отправка заданий клиентам

    for (auto& c : clients) {

        double start = current;
        double end = start + step * c.cores;

        if (&c == &clients.back())
            end = b;

        std::stringstream task;

        task << start << " " << end << " " << h << " " << method << "\n";

        send(c.socket, task.str().c_str(), task.str().size(), 0);

        Logger::log("Отправлено: " +
            std::to_string(start) + " -> " +
            std::to_string(end));

        current = end;
    }

    // Получение результатов от клиентов

    double total = 0.0;

    for (auto& c : clients) {

        char buffer[256]{};
        recv(c.socket, buffer, sizeof(buffer), 0);

        double val = atof(buffer);

        total += val;

#ifdef _WIN32
        closesocket(c.socket);
#endif
    }

    Logger::log("Итоговый результат: " + std::to_string(total));
}
