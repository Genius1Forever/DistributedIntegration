#pragma once
#include <vector>
#pragma once
#include <vector>
#include <string>
#include <mutex>

/**
 * @brief Информация о подключённом клиенте
 */
struct ClientInfo {

    /// Сокет клиента
    int socket;

    /// Количество ядер CPU
    int cores;
};

/**
 * @brief Диапазон вычислений
 */
struct Range {

    /// Начало диапазона
    double start;

    /// Конец диапазона
    double end;
};

/**
 * @brief Класс сервера распределённых вычислений
 *
 * Сервер принимает подключения клиентов,
 * распределяет задачи численного интегрирования
 * и собирает результаты.
 */
class Server {
public:

    /**
     * @brief Конструктор сервера
     *
     * @param port Порт для прослушивания подключений
     */
    explicit Server(int port);

    /**
     * @brief Запуск сервера
     *
     * Ожидает клиентов, распределяет задания,
     * получает и суммирует результаты.
     */
    void start();

private:

    /// Порт сервера
    int port_;

    /// Список подключённых клиентов
    std::vector<ClientInfo> clients_;

    /// Мьютекс для защиты списка клиентов
    std::mutex clients_mtx_;

    /**
     * @brief Обработка подключения клиента
     *
     * @param clientSocket Сокет клиента
     */
    void handleClient(int clientSocket);

    /**
     * @brief Отправка диапазонов клиенту
     *
     * @param client Информация о клиенте
     * @param ranges Список диапазонов
     * @param h Шаг интегрирования
     */
    void sendRangesToClient(
        const ClientInfo& client,
        const std::vector<Range>& ranges,
        double h
    );
};
