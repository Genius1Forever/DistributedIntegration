#pragma once

#include <iostream>
#include <mutex>
#include <string>

/**
 * @brief Простой потокобезопасный логгер
 *
 * Используется для вывода сообщений в консоль.
 * Гарантирует, что сообщения не перемешаются при работе нескольких потоков.
 */
class Logger {
public:

    /**
     * @brief Выводит лог-сообщение в консоль
     *
     * @param msg Сообщение для вывода
     *
     * Использует std::mutex для синхронизации потоков.
     */
    static void log(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "[LOG] " << msg << std::endl;
    }

private:

    /// Мьютекс для синхронизации доступа к консоли
    static std::mutex mutex_;
};
