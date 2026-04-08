#ifndef MAILER_H
#define MAILER_H

#include <QString>
#include <QSslSocket>
#include <QDebug>

class Mailing {
public:
    Mailing() = default;
    static bool sendCode(const QString &toEmail, const QString &code);
    static void loadCredentials(); // Функция для загрузки данных

private:
    static QString authUser;
    static QString authPass;
};

#endif
