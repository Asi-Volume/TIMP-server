/**
 * @file mailer.h
 * @brief Модуль для отправки писем по почте.
 * @details Обеспечивает взаимодействие с SMTP-сервером.
 * Используется для отправки кодов восстановления пароля.
 */

#ifndef MAILER_H
#define MAILER_H

#include <QDebug>
#include <QSslSocket>
#include <QString>

/**
 * @class Mailing
 * @brief Статический класс для управления почтовой рассылкой.
 */
class Mailing
{
public:
    /**
     * @brief Конструктор по умолчанию.
     */
    Mailing() = default;

    /**
     * @brief Отправляет код подтверждения на указанный e-mail.
     * @param toEmail Адрес получателя.
     * @param code Сгенерированный код.
     * @param login Логин пользователя.
     * @return «true» при успешном принятии письма SMTP-сервером.
     */
    static bool sendCode(const QString &toEmail, const QString &code, const QString &login);

    /**
     * @brief Загружает данные почтового аккаунта из файла конфигурации .env.
     */
    static void loadCredentials();

private:
    static QString authUser;    ///< e-mail аккаунта, отправляющего письмо.
    static QString authPass;    ///< Пароль для доступа к почте.
};

#endif
