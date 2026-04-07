#ifndef SERVER_CONTROLLER_H
#define SERVER_CONTROLLER_H

#include <QMap>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "server_model.h"

class ServerController : public QObject
{
    Q_OBJECT

public:
    explicit ServerController(QObject *parent = nullptr);
    ~ServerController();
    bool startServer(quint16 port);
    void stopServer();

private slots:
    void slotNewConnection();
    void slotServerRead();
    void slotClientDisconnected();

private:
    void processMessage(const QString &message, QTcpSocket *socket);
    void sendResponse(QTcpSocket *socket, const QString &response);

    QTcpServer *m_tcpServer;
    QMap<QTcpSocket *, QString> m_buffer;
    ServerModel *m_model;
};

#endif
