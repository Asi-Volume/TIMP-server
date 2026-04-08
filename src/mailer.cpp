#include "mailer.h"

bool Mailing::sendCode(const QString &toEmail, const QString &code) {
    QString host = "smtp.mail.ru";
    int port = 465;
    QString user = "vlad.duev.80@mail.ru";
    QString pass = "Qf5EZlGKXNrL75azQXlm";

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
        if (!socket.waitForReadyRead(3000)) return false;

        QString response = socket.readAll();
        qDebug() << ">>" << cmd << " | Server:" << response.trimmed();

        return response.contains(expectedCode);
    };

    // smtp-рукопожатие
    if (!executeStep("EHLO localhost", "250")) return false; // приветствие

    // авторизация
    if (!executeStep("AUTH LOGIN", "334")) return false;
    if (!executeStep(user.toUtf8().toBase64(), "334")) return false;
    if (!executeStep(pass.toUtf8().toBase64(), "235")) { //235 - успех
        qDebug() << "Пароль не принят сервером";
        return false;
    }

    // конверт
    executeStep(QString("MAIL FROM:<%1>").arg(user), "250");
    executeStep(QString("RCPT TO:<%1>").arg(toEmail), "250");

    if (!executeStep("DATA", "354")) return false; // данные

    // тело письма
    QString message = "From: " + user + "\r\n" +
                      "To: " + toEmail + "\r\n" +
                      "Subject: Код восстановления\r\n" +
                      "Content-Type: text/plain; charset=\"utf-8\"\r\n" +
                      "\r\n" +
                      "Код восстановления для аккаунта: " + code + "\r\n\n" +
                      "Отправлено автоматически. Проект ТиМП." + "\r\n" +
                      "\r\n.\r\n";

    socket.write(message.toUtf8());
    if (socket.waitForReadyRead(3000)) {
        qDebug() << "After DATA:" << socket.readAll().trimmed();
    }

    executeStep(message, "250");
    executeStep("QUIT", "221");

    socket.close();
    return true;
}
