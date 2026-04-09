#include <QCoreApplication>
#include <QDebug>
#include "mailer.h"
#include "server_controller.h"

int main(int argc, char *argv[])
{
    // создаем объект приложения QT для запуска цикла событий
    QCoreApplication a(argc, argv);
    Mailing::loadCredentials();

    qDebug() << "Запуск TCP-сервера...";

    // создаем объект контроллера сервера
    ServerController server;

    // пытаемся запустить сервер
    if (server.startServer(33333)) {
        qDebug() << "Сервер запущен на порту 33333";
        qDebug() << "Доступные команды:";
        qDebug() << "  auth&login&password";
        qDebug() << "  reg&login&password&email";
        qDebug() << "  stat&login";
        qDebug() << "  check&login&task_number&variant&answer";
    } else {
        qDebug() << "Не удалось запустить сервер";
        return 1;
    }

    // запускаем цикл событий для ожидания подключения клиентов и обработки событий.
    return a.exec();
}
