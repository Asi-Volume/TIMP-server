#ifndef SERVER_MODEL_H
#define SERVER_MODEL_H

#include <QString>
#include "database.h"

enum class RequestType { AUTH, REG, STAT, CHECK, RECOVER_CODE, RECOVER_CONF, UNKNOWN };

class ServerModel
{
public:
    ServerModel();
    ~ServerModel();
    RequestType parseRequest(const QString &request);
    QString processAuth(const QString &login, const QString &password);
    QString processReg(const QString &login, const QString &password, const QString &email);
    QString processStat(const QString &login);
    QString processCheck(const QString &login, int taskNumber, int variant, const QString &answer);
    QString processRecoverRequest(const QString &email);
    QString processRecoverConfirm(const QString &email, const QString &code, const QString &newPass);

private:
    Database *db;
};

#endif
