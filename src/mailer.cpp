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

    if (!executeStep("EHLO localhost", "250"))
        return false;

    if (!executeStep("AUTH LOGIN", "334"))
        return false;
    if (!executeStep(authUser.toUtf8().toBase64(), "334"))
        return false;
    if (!executeStep(authPass.toUtf8().toBase64(), "235")) {
        qDebug() << "Пароль не принят сервером";
        return false;
    }

    executeStep(QString("MAIL FROM:<%1>").arg(authUser), "250");
    executeStep(QString("RCPT TO:<%1>").arg(toEmail), "250");

    if (!executeStep("DATA", "354"))
        return false;

    QString htmlBody = QString(R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #f4f7f6; margin: 0; padding: 40px 20px; }
        .email-container { max-width: 600px; margin: 0 auto; background-color: #ffffff; padding: 40px; border-radius: 12px; box-shadow: 0 4px 15px rgba(0,0,0,0.05); }
        .header { text-align: center; color: #333333; font-size: 24px; font-weight: bold; margin-bottom: 20px; }
        .greeting { font-size: 18px; color: #555555; margin-bottom: 15px; }
        .message-text { font-size: 16px; color: #666666; line-height: 1.6; margin-bottom: 25px; }
        .code-box { background-color: #f8f9fa; border: 2px dashed #007bff; color: #007bff; font-size: 32px; font-weight: bold; text-align: center; padding: 20px; border-radius: 8px; letter-spacing: 4px; margin-bottom: 30px; }
        .footer { border-top: 1px solid #eeeeee; padding-top: 20px; font-size: 13px; color: #999999; text-align: center; line-height: 1.5; }
    </style>
</head>
<body>
    <div class="email-container">
        <div class="header">Восстановление доступа</div>
        <div class="greeting">Здравствуйте, <b>%1</b>!</div>
        <div class="message-text">
            Мы получили запрос на восстановление доступа к вашему аккаунту.
            Пожалуйста, используйте следующий код для продолжения:
        </div>
        <div class="code-box">%2</div>
        <div class="message-text" style="font-size: 14px;">
            Если вы не запрашивали этот код, просто проигнорируйте данное письмо. Ваши данные в безопасности.
        </div>
        <div class="footer">
            Это письмо отправлено автоматически, отвечать на него не нужно.<br>
            <b>Проект ТиМП</b>
        </div>
    </div>
</body>
</html>
)").arg(login, code);

    QString message = "From: " + authUser + "\r\n"
                      + "To: " + toEmail + "\r\n"
                      + "Subject: Код восстановления аккаунта\r\n"
                      + "MIME-Version: 1.0\r\n"
                      + "Content-Type: text/html; charset=\"utf-8\"\r\n\r\n"
                      + htmlBody + "\r\n.\r\n";

    socket.write(message.toUtf8());
    if (socket.waitForReadyRead(3000)) {
        qDebug() << "After DATA:" << socket.readAll().trimmed();
    }

    executeStep("QUIT", "221");

    socket.close();
    return true;
}
