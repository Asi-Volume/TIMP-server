#include "mailer.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>

QString Mailing::authUser = "";
QString Mailing::authPass = "";

void Mailing::loadCredentials()
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString fullPath = appDir + "/.env";
    qDebug() << "--- Поиск конфига ---";
    qDebug() << "Рабочая директория:" << QDir::currentPath();
    qDebug() << "Ожидаемый путь к .env:" << fullPath;
    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Файл .env не найден по пути:" << fullPath;
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("EMAIL=")) {
            authUser = line.section('=', 1).trimmed();
        } else if (line.startsWith("PASSWORD=")) {
            authPass = line.section('=', 1).trimmed();
        }
    }
    file.close();
}

bool Mailing::sendCode(const QString &toEmail, const QString &code, const QString &login)
{
    QString host = "smtp.mail.ru";
    int port = 465;

    if (authUser.isEmpty() || authPass.isEmpty()) {
        qDebug() << "Данные из .env не загружены";
        return false;
    }

    QSslSocket socket;
    socket.connectToHostEncrypted(host, port);

    if (!socket.waitForEncrypted(3000)) {
        qDebug() << "Ошибка SSL: " << socket.errorString();
        return false;
    }

    if (socket.waitForReadyRead(3000)) {
        qDebug() << "Приветствие: " << socket.readAll().trimmed();
    }

    auto executeStep = [&](const QString &cmd, const QString &expectedCode) {
        socket.write(cmd.toUtf8() + "\r\n");
        if (!socket.waitForReadyRead(3000))
            return false;

        QString response = socket.readAll();
        qDebug() << ">>" << cmd << " | Server:" << response.trimmed();

        return response.contains(expectedCode);
    };

    // smtp-рукопожатие
    if (!executeStep("EHLO localhost", "250"))
        return false; // приветствие

    // авторизация
    if (!executeStep("AUTH LOGIN", "334"))
        return false;
    if (!executeStep(authUser.toUtf8().toBase64(), "334"))
        return false;
    if (!executeStep(authPass.toUtf8().toBase64(), "235")) { //235 - успех
        qDebug() << "Пароль не принят сервером";
        return false;
    }

    // конверт
    executeStep(QString("MAIL FROM:<%1>").arg(authUser), "250");
    executeStep(QString("RCPT TO:<%1>").arg(toEmail), "250");

    if (!executeStep("DATA", "354"))
        return false; // данные

    // тело письма
    QString message = "From: " + authUser + "\r\n" + "To: " + toEmail + "\r\n"
                      + "Subject: Код восстановления\r\n"
                      + "Content-Type: text/plain; charset=\"utf-8\"\r\n" + "\r\n"
                      + "Здравствуйте, " + login + "\r\n\n"
                      + "Код восстановления для аккаунта: " + code + "\r\n\n"
                      + "Отправлено автоматически. Проект ТиМП." + "\r\n" + "\r\n.\r\n";

    socket.write(message.toUtf8());
    if (socket.waitForReadyRead(3000)) {
        qDebug() << "After DATA:" << socket.readAll().trimmed();
    }

    executeStep("QUIT", "221");

    socket.close();
    return true;
}
