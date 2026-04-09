#ifndef MAILER_H
#define MAILER_H

#include <QDebug>
#include <QSslSocket>
#include <QString>

class Mailing
{
public:
    Mailing() = default;
    static bool sendCode(const QString &toEmail, const QString &code, const QString &login);
    static void loadCredentials(); // Функция для загрузки данных

private:
    static QString authUser;
    static QString authPass;
};

#endif
