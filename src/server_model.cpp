#include "server_model.h"
#include <QMap>
#include <QRandomGenerator>
#include "mailer.h"

static QMap<QString, QString> tempCodes;

ServerModel::ServerModel()
{
    db = Database::getInstance();
    db->connect();
}

ServerModel::~ServerModel() {}

RequestType ServerModel::parseRequest(const QString &request)
{
    if (request.startsWith("auth&"))
        return RequestType::AUTH;
    if (request.startsWith("reg&"))
        return RequestType::REG;
    if (request.startsWith("recover_code&"))
        return RequestType::RECOVER_CODE;
    if (request.startsWith("recover_conf&"))
        return RequestType::RECOVER_CONF;
    return RequestType::UNKNOWN;
}

QString ServerModel::processAuth(const QString &login, const QString &password)
{
    return db->authUser(login, password) ? "auth+&" + login : "auth-";
}

QString ServerModel::processReg(const QString &login, const QString &password, const QString &email)
{
    return db->regUser(login, password, email) ? "reg+&" + login : "reg-";
}

QString ServerModel::processRecoverRequest(const QString &email)
{
    if (!db->emailExists(email))
        return "recover_code-";

    QString code = QString::number(QRandomGenerator::global()->bounded(100000, 999999));
    QString login = db->getLoginByEmail(email);
    tempCodes[email] = code;

    if (Mailing::sendCode(email, code, login)) {
        return "recover_code+";
    } else {
        qDebug() << "Ошибка отправки письма!";
        return "recover_code-";
    }
}

QString ServerModel::processRecoverConfirm(const QString &email,
                                           const QString &code,
                                           const QString &newPass)
{
    if (tempCodes.value(email) == code && !code.isEmpty()) {
        if (db->updatePasswordByEmail(email, newPass)) {
            tempCodes.remove(email);
            return "recover_conf+";
        }
    }
    return "recover_conf-";
}
