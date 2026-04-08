#include "database.h"

Database *Database::instance = nullptr;

Database::Database() {}
Database::~Database()
{
    disconnect();
}

Database *Database::getInstance()
{
    if (instance == nullptr) {
        instance = new Database();
    }
    return instance;
}

bool Database::connect()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("server_database.db");
    if (!db.open()) {
        qDebug() << "Ошибка БД:" << db.lastError().text();
        return false;
    }
    createTables();
    qDebug() << "БД подключена";
    return true;
}

void Database::disconnect()
{
    if (db.isOpen())
        db.close();
}

void Database::createTables()
{
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "login TEXT UNIQUE NOT NULL,"
               "password TEXT NOT NULL,"
               "email TEXT NOT NULL,"
               "total_solved INTEGER DEFAULT 0,"
               "total_attempts INTEGER DEFAULT 0,"
               "rating INTEGER DEFAULT 0)");

    query.exec("CREATE TABLE IF NOT EXISTS tasks ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "task_number INTEGER NOT NULL,"
               "variant INTEGER NOT NULL,"
               "correct_answer TEXT NOT NULL)");

    QSqlQuery checkQuery;
    checkQuery.exec("SELECT COUNT(*) FROM tasks");
    checkQuery.next();
    if (checkQuery.value(0).toInt() == 0) {
        query.prepare(
            "INSERT INTO tasks (task_number, variant, correct_answer) VALUES (1, 1, '5')");
        query.exec();
        query.prepare(
            "INSERT INTO tasks (task_number, variant, correct_answer) VALUES (1, 2, '7')");
        query.exec();
        query.prepare(
            "INSERT INTO tasks (task_number, variant, correct_answer) VALUES (2, 1, '12')");
        query.exec();
        query.prepare(
            "INSERT INTO tasks (task_number, variant, correct_answer) VALUES (2, 2, '20')");
        query.exec();
    }
}

bool Database::regUser(const QString &login, const QString &password, const QString &email)
{
    QSqlQuery query;
    QByteArray hashedPassword
        = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
    query.prepare("INSERT INTO users (login, password, email) VALUES (:login, :password, :email)");
    query.bindValue(":login", login);
    query.bindValue(":password", QString(hashedPassword));
    query.bindValue(":email", email);
    return query.exec();
}

bool Database::authUser(const QString &login, const QString &password)
{
    QSqlQuery query;
    QByteArray hashedPassword
        = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
    query.prepare("SELECT password FROM users WHERE login = :login");
    query.bindValue(":login", login);
    if (query.exec() && query.next()) {
        return query.value(0).toString() == QString(hashedPassword);
    }
    return false;
}

UserData Database::getStats(const QString &login)
{
    UserData data;
    data.login = login;
    data.totalSolved = 0;
    data.totalAttempts = 0;
    data.currentRating = 0;
    QSqlQuery query;
    query.prepare("SELECT total_solved, total_attempts, rating FROM users WHERE login = :login");
    query.bindValue(":login", login);
    if (query.exec() && query.next()) {
        data.totalSolved = query.value(0).toInt();
        data.totalAttempts = query.value(1).toInt();
        data.currentRating = query.value(2).toInt();
    }
    return data;
}

QString Database::getCorrectAnswer(int taskNumber, int variant)
{
    QSqlQuery query;
    query.prepare(
        "SELECT correct_answer FROM tasks WHERE task_number = :task_number AND variant = :variant");
    query.bindValue(":task_number", taskNumber);
    query.bindValue(":variant", variant);
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return "";
}

bool Database::checkSolution(const QString &login,
                             int taskNumber,
                             int variant,
                             const QString &answer)
{
    QString correctAnswer = getCorrectAnswer(taskNumber, variant);
    bool isCorrect = (answer == correctAnswer);
    updateStats(login, isCorrect);
    return isCorrect;
}

void Database::updateStats(const QString &login, bool isCorrect)
{
    QSqlQuery query;
    if (isCorrect) {
        query.prepare("UPDATE users SET total_solved = total_solved + 1, total_attempts = "
                      "total_attempts + 1, rating = rating + 10 WHERE login = :login");
    } else {
        query.prepare("UPDATE users SET total_attempts = total_attempts + 1, rating = rating - 5 "
                      "WHERE login = :login");
    }
    query.bindValue(":login", login);
    query.exec();
}
