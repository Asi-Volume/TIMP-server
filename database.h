#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDebug>
#include <QCryptographicHash>

struct UserData {
    QString login;
    QString password;
    QString email;
    int totalSolved;
    int totalAttempts;
    int currentRating;
};

class Database
{
public:
    static Database* getInstance();
    bool connect();
    void disconnect();
    bool regUser(const QString& login, const QString& password, const QString& email);
    bool authUser(const QString& login, const QString& password);
    UserData getStats(const QString& login);
    bool checkSolution(const QString& login, int taskNumber, int variant, const QString& answer);
    void updateStats(const QString& login, bool isCorrect);

private:
    Database();
    ~Database();
    void createTables();
    QString getCorrectAnswer(int taskNumber, int variant);

    static Database* instance;
    QSqlDatabase db;
};

#endif