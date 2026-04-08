#include <QCoreApplication>
#include <QDebug>
#include "server_controller.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Запуск TCP-сервера...";

    ServerController server;

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

    return a.exec();
}
