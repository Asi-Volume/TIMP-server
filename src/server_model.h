/**
 * @file server_model.h
 * @brief Описание логики обработки запросов.
 * @details Класс, который нтерпретирует команды от клиента и
 * вызывает соответствующие методы БД или почтового сервиса.
 */
#ifndef SERVER_MODEL_H
#define SERVER_MODEL_H

#include <QString>
#include "database.h"

/**
 * @enum RequestType
 * @brief Типы запросов, которые сервер может получить от клиента.
 */
enum class RequestType {
    AUTH,               ///< Авторизация пользователя
    REG,                ///< Регистрация нового аккаунта
    STAT,               ///< Запрос статистики
    CHECK,              ///< Проверка ответа на задачу
    RECOVER_CODE,       ///< Запрос кода восстановления на почту
    RECOVER_CONF,       ///< Подтверждение смены пароля кодом
    UNKNOWN             ///< Неизвестная команда
};


/**
 * @class ServerModel
 * @brief Класс, реализующий модель сервера.
 */
class ServerModel
{
public:
    /**
     * @brief Конструктор для инициализации соединение с БД.
     */
    ServerModel();

    /**
     * @brief Деструктор.
     */
    ~ServerModel();

    /**
     * @brief Определяет тип входящего запроса по его префиксу.
     * @param request Полная строка запроса от клиента.
     * @return Тип запроса в формате RequestType.
     */
    RequestType parseRequest(const QString &request);

    /**
     * @brief Обработка попытки входа.
     */
    QString processAuth(const QString &login, const QString &password);

    /**
     * @brief Обработка регистрации нового пользователя.
     */
    QString processReg(const QString &login, const QString &password, const QString &email);

    /**
     * @brief Формирование строки со статистикой пользователя.
     */
    QString processStat(const QString &login);

    /**
     * @brief Проверка правильности ответа на задачу и обновление БД.
     */
    QString processCheck(const QString &login, int taskNumber, int variant, const QString &answer);

    /**
     * @brief Генерация кода и отправка на почту пользователя.
     * @param login Логин пользователя.
     */
    QString processRecoverRequest(const QString &email);

    /**
     * @brief Проверка кода из письма и установка нового пароля.
     * @param email Почта пользователя.
     * @param code Код, который пользователь ввел в приложении.
     * @param newPass Новый пароль.
     */
    QString processRecoverConfirm(const QString &email, const QString &code, const QString &newPass);

private:
    Database *db;   ///< Указатель на экземпляр базы данных
};

#endif
