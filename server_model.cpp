#include "server_model.h"

ServerModel::ServerModel()
{
    db = Database::getInstance();
    db->connect();
}

ServerModel::~ServerModel() {}

RequestType ServerModel::parseRequest(const QString& request)
{
    if (request.startsWith("auth&")) return RequestType::AUTH;
    if (request.startsWith("reg&")) return RequestType::REG;
    if (request.startsWith("stat&")) return RequestType::STAT;
    if (request.startsWith("check&")) return RequestType::CHECK;
    return RequestType::UNKNOWN;
}

QString ServerModel::processAuth(const QString& login, const QString& password)
{
    return db->authUser(login, password) ? "auth+&" + login : "auth-";
}

QString ServerModel::processReg(const QString& login, const QString& password, const QString& email)
{
    return db->regUser(login, password, email) ? "reg+&" + login : "reg-";
}

QString ServerModel::processStat(const QString& login)
{
    UserData data = db->getStats(login);
    return QString("stat&%1&%2&%3").arg(data.totalSolved).arg(data.totalAttempts).arg(data.currentRating);
}

QString ServerModel::processCheck(const QString& login, int taskNumber, int variant, const QString& answer)
{
    return db->checkSolution(login, taskNumber, variant, answer) ? "check+" : "check-";
}