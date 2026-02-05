#include "Client.h"
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

    std::cout << "Клиент запущен\n";

    Client client("127.0.0.1", 5555);
    client.start();

    return 0;
}
