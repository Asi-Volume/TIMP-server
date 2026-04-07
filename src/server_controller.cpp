#include "server_controller.h"
#include <QDebug>

ServerController::ServerController(QObject *parent)
    : QObject(parent)
{
    m_tcpServer = new QTcpServer(this);
    m_model = new ServerModel();
}

ServerController::~ServerController()
{
    stopServer();
    delete m_model;
}

bool ServerController::startServer(quint16 port)
{
    connect(m_tcpServer, &QTcpServer::newConnection, this, &ServerController::slotNewConnection);

    if (!m_tcpServer->listen(QHostAddress::Any, port)) {
        qDebug() << "������ ������� ������� �� �����" << port;
        return false;
    }

    qDebug() << "������ ������� �� �����" << port;
    return true;
}

void ServerController::stopServer()
{
    if (m_tcpServer->isListening()) {
        for (QTcpSocket *socket : m_buffer.keys()) {
            socket->close();
        }
        m_buffer.clear();
        m_tcpServer->close();
        qDebug() << "������ ����������";
    }
}

void ServerController::slotNewConnection()
{
    QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();
    qDebug() << "����� ����������� ��" << clientSocket->peerAddress().toString();

    connect(clientSocket, &QTcpSocket::readyRead, this, &ServerController::slotServerRead);
    connect(clientSocket,
            &QTcpSocket::disconnected,
            this,
            &ServerController::slotClientDisconnected);

    m_buffer[clientSocket] = "";
}

void ServerController::slotServerRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket)
        return;

    QByteArray data = clientSocket->readAll();
    m_buffer[clientSocket] += QString::fromUtf8(data);

    if (m_buffer[clientSocket].contains('\n')) {
        QStringList messages = m_buffer[clientSocket].split('\n', Qt::SkipEmptyParts);
        m_buffer[clientSocket] = "";

        for (const QString &msg : messages) {
            processMessage(msg.trimmed(), clientSocket);
        }
    }
}

void ServerController::processMessage(const QString &message, QTcpSocket *socket)
{
    qDebug() << "���������:" << message;

    RequestType type = m_model->parseRequest(message);
    QString response;

    switch (type) {
    case RequestType::AUTH: {
        QStringList parts = message.split('&');
        if (parts.size() >= 3) {
            response = m_model->processAuth(parts[1], parts[2]);
        } else {
            response = "auth-";
        }
        break;
    }
    case RequestType::REG: {
        QStringList parts = message.split('&');
        if (parts.size() >= 4) {
            response = m_model->processReg(parts[1], parts[2], parts[3]);
        } else {
            response = "reg-";
        }
        break;
    }
    case RequestType::STAT: {
        QStringList parts = message.split('&');
        if (parts.size() >= 2) {
            response = m_model->processStat(parts[1]);
        } else {
            response = "stat&0&0&0";
        }
        break;
    }
    case RequestType::CHECK: {
        QStringList parts = message.split('&');
        if (parts.size() >= 5) {
            response = m_model->processCheck(parts[1], parts[2].toInt(), parts[3].toInt(), parts[4]);
        } else {
            response = "check-";
        }
        break;
    }
    default:
        response = "Unknown command";
        break;
    }

    sendResponse(socket, response);
}

void ServerController::sendResponse(QTcpSocket *socket, const QString &response)
{
    socket->write((response + "\n").toUtf8());
    socket->flush();
    qDebug() << "�����:" << response;
}

void ServerController::slotClientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (clientSocket) {
        qDebug() << "������ ����������";
        m_buffer.remove(clientSocket);
        clientSocket->deleteLater();
    }
}
