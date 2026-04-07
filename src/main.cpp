#include <QCoreApplication>
#include <QDebug>
#include "server_controller.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "������ TCP-�������...";

    ServerController server;

    if (server.startServer(33333)) {
        qDebug() << "������ ������� �� ����� 33333";
        qDebug() << "��������� �������:";
        qDebug() << "  auth&login&password";
        qDebug() << "  reg&login&password&email";
        qDebug() << "  stat&login";
        qDebug() << "  check&login&task_number&variant&answer";
    } else {
        qDebug() << "�� ������� ��������� ������";
        return 1;
    }

    return a.exec();
}
