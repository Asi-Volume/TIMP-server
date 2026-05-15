/**
 * @file database.h
 * @brief Класс для работы с БД SQLite.
 * @details Реализует паттерн Singleton для управления пользователями и статистикой.
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <QCryptographicHash>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>

/**
 * @struct UserData
 * @brief Структура для хранения краткой информации о пользователе.
 */

struct UserData
{
    QString login;      ///< Логин пользователя
    QString password;   ///< Хеш пароля
    QString email;      ///< Электронная почта
    int totalSolved;    ///< Общее количество решенных задач
    int totalAttempts;  ///< Общее количество попыток
    int currentRating;  ///< Текущий рейтинг в системе
};

/**
 * @class Database
 * @brief Управляет всеми операциями с БД.
 */
class Database
{
public:
    /**
     * @brief Получение единственного экземпляра класса.
     */
    static Database *getInstance();

    /**
     * @brief Инициализация и открытие соединения с БД.
     * @return «true» при успешном подключении.
     */
    bool connect();

    /**
     * @brief Закрытие соединения с БД.
     */
    void disconnect();

    /**
     * @brief Регистрация нового пользователя.
     * @param login Логин пользователя.
     * @param password Пароль.
     * @param email Почта для восстановления доступа.
     * @return «true» при успешной записи.
     */
    bool regUser(const QString &login, const QString &password, const QString &email);

    /**
     * @brief Проверка учетных данных при входе.
     */
    bool authUser(const QString &login, const QString &password);

    /**
     * @brief Получение статистики пользователя по логину.
     */
    UserData getStats(const QString &login);

    /**
     * @brief Получение логина, привязанного к e-mail.
     */
    QString getLoginByEmail(const QString &email);

    /**
     * @brief Проверка решения задачи и обновление статистики.
     * @param taskNumber Номер задачи.
     * @param variant Номер варианта.
     * @param answer Ответ пользователя.
     */
    bool checkSolution(const QString &login, int taskNumber, int variant, const QString &answer);


    /**
     * @brief Обновление статистики пользователя.
     */
    void updateStats(const QString &login, bool isCorrect);

    /**
     * @brief Проверка существования e-mail в базе.
     */
    bool emailExists(const QString &email);

    /**
     * @brief Проверка существования логина в базе.
     */
    bool loginExists(const QString &login);

    /**
     * @brief Обновление пароля пользователя по Email.
     */
    bool updatePasswordByEmail(const QString &email, const QString &newPass);

    /**
     * @brief Получение e-mail, привязанного к логину.
     */
    QString getEmailByLogin(const QString &login);

private:
    Database();
    ~Database();

    /**
     * @brief Создание необходимых таблиц в случае их отсутствия.
     */
    void createTables();

    /**
     * @brief Внутренняя функция для получения правильного ответа из таблицы задач.
     */
    QString getCorrectAnswer(int taskNumber, int variant);

    static Database *instance;
    QSqlDatabase db;
};

#endif
