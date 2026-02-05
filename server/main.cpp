#include "Server.h"
#include <iostream>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {

    // Русификация консоли
    setlocale(LC_ALL, "");
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif

    std::cout << "Сервер запущен\n";
    Server server(5555);
    server.start();

    return 0;
}