#include <QtTest>
#include "server_model.h"
#include "mailer.h"

class FuncForServer_Test : public QObject
{
    Q_OBJECT
private:
    ServerModel *server;

public:
    FuncForServer_Test() : server(nullptr) {}
    ~FuncForServer_Test() {}

private slots:
    void initTestCase() {
        Mailing::loadCredentials();
        server = new ServerModel();
    }
    void cleanupTestCase() {
        delete server;
        QSqlQuery query;
        query.prepare("DELETE FROM users WHERE login LIKE 'test_%'");
        query.exec();
    }

    // Тест неизвестной команды
    void test_unknown()
    {
        RequestType type = server->parseRequest("asdf");
        QVERIFY(type == RequestType::UNKNOWN);
    }
    // Тест регистрации
    void test_reg()
    {
        RequestType type = server->parseRequest("reg&test_user1&pass&test_user1@mail.ru");
        QVERIFY(type == RequestType::REG);
    }
    // Тест авторизации
    void test_auth()
    {
        RequestType type = server->parseRequest("auth&test_user1&pass");
        QVERIFY(type == RequestType::AUTH);
    }

    // Тест пользовательского поведения
    void test_complex()
    {
        QString testLogin = "test_user2";
        QString testPass = "password";
        QString testEmail = "test_user2@bk.ru";

        // Проверка регистрации (reg+&логин)
        QString regResult = server->processReg(testLogin, testPass, testEmail);
        QCOMPARE(regResult, QString("reg+&") + testLogin);

        // Проверка регистрации на те же данные (reg-)
        QString regRepeatResult = server->processReg(testLogin, "passkey", "test_mail@mail.ru");
        QCOMPARE(regRepeatResult, QString("reg-"));

        // Проверка успешной авторизации (auth+&логин)
        QString authResult = server->processAuth(testLogin, testPass);
        QCOMPARE(authResult, QString("auth+&") + testLogin);

        // Проверка входа с существующим логином и неверным паролем (auth-)
        QString authWrongResult = server->processAuth(testLogin, "wrong");
        QCOMPARE(authWrongResult, QString("auth-"));
    }
};

QTEST_MAIN(FuncForServer_Test)

#include "tst_funcforserver_test.moc"
