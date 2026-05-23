#include <QtTest>
#include "server_model.h"

class FuncForServer_Test : public QObject
{
    Q_OBJECT

public:
    FuncForServer_Test() {}
    ~FuncForServer_Test() {}

private slots:
    // Тест неизвестной команды
    void test_parseRequest_unknown()
    {
        ServerModel server;
        RequestType type = server.parseRequest("asdf");
        QVERIFY(type == RequestType::UNKNOWN);
    }
    // Тест регистрации
    void test_parseRequest_reg()
    {
        ServerModel server;
        RequestType type = server.parseRequest("reg&user1&pass&user1@mail.ru");
        QVERIFY(type == RequestType::REG);
    }
    // Тест авторизации
    void test_parseRequest_auth()
    {
        ServerModel server;
        RequestType type = server.parseRequest("auth&user1&pass");
        QVERIFY(type == RequestType::AUTH);
    }

    // Тест пользовательского поведения
    void test_user_lifecycle_scenario()
    {
        ServerModel model;

        QString testLogin = "user2";
        QString testPass = "password";
        QString testEmail = "user2@bk.ru";

        // Проверка регистрации (reg+&логин)
        QString regResult = model.processReg(testLogin, testPass, testEmail);
        QCOMPARE(regResult, QString("reg+&") + testLogin);

        // Проверка регистрации на те же данные (reg-)
        QString regRepeatResult = model.processReg(testLogin, "passkey", "tester@mail.ru");
        QCOMPARE(regRepeatResult, QString("reg-"));

        // Проверка успешной авторизации (auth+&логин)
        QString authResult = model.processAuth(testLogin, testPass);
        QCOMPARE(authResult, QString("auth+&") + testLogin);

        // Проверка входа с существующим логином и неверным паролем (auth-)
        QString authWrongResult = model.processAuth(testLogin, "wrong");
        QCOMPARE(authWrongResult, QString("auth-"));
    }
};

QTEST_APPLESS_MAIN(FuncForServer_Test)

#include "tst_funcforserver_test.moc"
