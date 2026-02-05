#include "Client.h"
#include "../common/Logger.h"

#include <thread>
#include <vector>
#include <cmath>
#include <sstream>
#include <mutex>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

/**
 * @brief Численное интегрирование методом трапеций
 *
 * @param a Нижний предел
 * @param b Верхний предел
 * @param h Шаг интегрирования
 * @return Значение интеграла
 */
double integrateTrapezoid(double a, double b, double h) {

    double sum = 0.0;

    for (double x = a; x < b; x += h) {

        double x2 = x + h;

        sum += (1.0 / std::log(x) +
            1.0 / std::log(x2)) * h / 2.0;
    }

    return sum;
}

/**
 * @brief Численное интегрирование методом Симпсона
 *
 * @param a Нижний предел
 * @param b Верхний предел
 * @param h Шаг интегрирования
 * @return Значение интеграла
 */
double integrateSimpson(double a, double b, double h) {

    int n = static_cast<int>((b - a) / h);

    /* Для Симпсона нужно чётное количество шагов */
    if (n % 2 != 0) {
        n++;
    }

    double step = (b - a) / n;

    double sum = 0.0;

    for (int i = 0; i <= n; i++) {

        double x = a + i * step;
        double fx = 1.0 / std::log(x);

        if (i == 0 || i == n) {
            sum += fx;
        }
        else if (i % 2 == 0) {
            sum += 2 * fx;
        }
        else {
            sum += 4 * fx;
        }
    }

    return sum * step / 3.0;
}

Client::Client(const std::string& host, int port)
    : host_(host), port_(port) {
}

/**
 * @brief Запуск клиента
 *
 * Подключается к серверу, сообщает количество ядер,
 * выполняет интегрирование в потоках и отправляет результат серверу.
 */
void Client::start() {

#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = inet_addr(host_.c_str());

    connect(sock, (sockaddr*)&addr, sizeof(addr));

    /* Отправляем количество ядер */
    int cores = std::thread::hardware_concurrency();
    std::string coresMsg = "CORES " + std::to_string(cores) + "\n";
    send(sock, coresMsg.c_str(), coresMsg.size(), 0);

    Logger::log("Отправлено ядер: " + std::to_string(cores));

    /* Получаем задание */
    char buffer[256]{};
    recv(sock, buffer, sizeof(buffer), 0);

    double a, b, h;
    int method;


    std::stringstream ss(buffer);
    ss >> a >> b >> h >> method;

    Logger::log("Получено задание: a=" + std::to_string(a) +
        " b=" + std::to_string(b));

    /* Параллельный расчёт */

    double result = 0.0;
    std::mutex mtx;

    double chunk = (b - a) / cores;

    std::vector<std::thread> threads;

    /**
     * @brief Лямбда для вычисления интеграла в отдельном потоке
     */

    for (int i = 0; i < cores; i++) {

        double start = a + i * chunk;
        double end = (i == cores - 1) ? b : start + chunk;

        threads.emplace_back([&, start, end]() {

            int method = 2; // 1 - трапеции, 2 - Симпсон

            double local;

            if (method == 1)
                local = integrateTrapezoid(start, end, h);
            else
                local = integrateSimpson(start, end, h);

            std::lock_guard<std::mutex> lock(mtx);
            result += local;

            });
    }

    for (auto& t : threads)
        t.join();

    /* Отправляем результат */

    std::stringstream out;
    out << result << "\n";

    send(sock, out.str().c_str(), out.str().size(), 0);

    Logger::log("Результат отправлен серверу");

#ifdef _WIN32
    closesocket(sock);
#endif
}
