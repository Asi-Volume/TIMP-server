/**
 * @file main.cpp
 * @brief Точка входа в серверное приложение.
 * @details Инициализирует окружение Qt, загружает учетные данные почты
 * и запускает TCP-сервер на прослушивание порта.
 */

#include <QCoreApplication>
#include <QDebug>
#include "mailer.h"
#include "server_controller.h"

/**
 * @brief Главная функция сервера.
 * @param argc Количество аргументов командной строки.
 * @param argv Массив аргументов командной строки.
 * @return Код завершения приложения.
 */
int main(int argc, char *argv[])
{
    // Создание объекта приложения Qt для управления циклом событий
    QCoreApplication a(argc, argv);

    // Загружаем настройки SMTP (логин/пароль) из файла .env
    Mailing::loadCredentials();

    qDebug() << "Запуск TCP-сервера...";

    // Инициализация контроллера, который создаст серверный сокет
    ServerController server;

    // Открытие порта 33333 для входящих соединений
    if (server.startServer(33333)) {
        qDebug() << "Сервер запущен на порту 33333";
        qDebug() << "Доступные команды:";
        qDebug() << "  auth&login&password";
        qDebug() << "  reg&login&password&email";
        qDebug() << "  recover_code&email";
        qDebug() << "  recover_confirm&email&code&new_password";
        qDebug() << "  stat&login";
        qDebug() << "  check&login&task_number&variant&answer";
    } else {
        qDebug() << "Не удалось запустить сервер";
        return 1;
    }

    // Запуск бесконечной обработки событий
    return a.exec();
}
