#include "server_controller.h"
#include <QDebug>

ServerController::ServerController(QObject *parent)
    : QObject(parent)
{
    // создаем TCP сервер и модель, которая будет обрабатывать логику запросов
    m_tcpServer = new QTcpServer(this);
    m_model = new ServerModel();
}

ServerController::~ServerController()
{
    // при уничтожении контролерра останавливаем сервер и освобождаем память
    stopServer();
    delete m_model;
}

bool ServerController::startServer(quint16 port)
{
    // подключаем сигнал нового входящего соединения к слоту обработки подключения
    connect(m_tcpServer, &QTcpServer::newConnection, this, &ServerController::slotNewConnection);

    // пытаемся запустить сервер на указанном порту
    if (!m_tcpServer->listen(QHostAddress::Any, port)) {
        qDebug() << "Не удалось запустить сервер на порту" << port;
        return false;
    }

    qDebug() << "Сервер запущен на" << port;
    return true;
}

void ServerController::stopServer()
{
    // проверка работает ли сервер
    if (m_tcpServer->isListening()) {
        // закрываем все клиентские сокеты
        for (QTcpSocket *socket : m_buffer.keys()) {
            socket->close();
        }
        // очищаем буфер сообщений и выключаем сервер
        m_buffer.clear();
        m_tcpServer->close();
        qDebug() << "Сервер прекратил работу";
    }
}

void ServerController::slotNewConnection()
{
    // получаем сокет нового подключившегося клиента
    QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();
    qDebug() << "Новое подключение от" << clientSocket->peerAddress().toString();

    // подключаем сигнал readyRead, он сработает когда прийдут данные
    connect(clientSocket, &QTcpSocket::readyRead, this, &ServerController::slotServerRead);
    // подключаем сигнал отключения клиента
    connect(clientSocket,
            &QTcpSocket::disconnected,
            this,
            &ServerController::slotClientDisconnected);

    // для нового клиента создаем пустой буфер
    m_buffer[clientSocket] = "";
}

void ServerController::slotServerRead()
{
    // определяем от какого сокета пришел сигнал reaadyRead
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket)
        return;

    // считываем все пришедшие данные и добавляем в буфер этого клиента
    QByteArray data = clientSocket->readAll();
    m_buffer[clientSocket] += QString::fromUtf8(data);

    // если в буфере есть символ конца строки, то это значит что сообщение пришло полностью
    if (m_buffer[clientSocket].contains('\n')) {
        // разбиваем буфер на отдельные сообщения
        QStringList messages = m_buffer[clientSocket].split('\n', Qt::SkipEmptyParts);
        // очищаем буфер после разбиения
        m_buffer[clientSocket] = "";

        // обрабатываем каждое сообщение отдельно
        for (const QString &msg : messages) {
            processMessage(msg.trimmed(), clientSocket);
        }
    }
}

void ServerController::processMessage(const QString &message, QTcpSocket *socket)
{
    qDebug() << "Получено сообщение:" << message;

    // определяем тип запроса
    RequestType type = m_model->parseRequest(message);
    // переменная для ответа клиенту
    QString response;

    switch (type) {
    case RequestType::AUTH: {
        // формат auth&login&password
        QStringList parts = message.split('&');
        if (parts.size() >= 3) {
            response = m_model->processAuth(parts[1], parts[2]);
        } else {
            response = "auth-";
        }
        break;
    }
    case RequestType::REG: {
        // формат reg&login&password&email
        QStringList parts = message.split('&');
        if (parts.size() >= 4) {
            response = m_model->processReg(parts[1], parts[2], parts[3]);
        } else {
            response = "reg-";
        }
        break;
    }
    case RequestType::RECOVER_CODE: {
        // формат recover_code&email
        QStringList parts = message.split('&');
        response = (parts.size() >= 2) ? m_model->processRecoverRequest(parts[1]) : "recover_code-";
        break;
    }
    case RequestType::RECOVER_CONF: {
        // формат recover_confirm&email&code&new_password
        QStringList parts = message.split('&');
        if (parts.size() >= 4) {
            response = m_model->processRecoverConfirm(parts[1], parts[2], parts[3]);
        } else {
            response = "recover_conf-";
        }
        break;
    }
    default:
        response = "Unknown command";
        break;
    }


    // отправляем сформированный ответ клиенту
    sendResponse(socket, response);
}

void ServerController::sendResponse(QTcpSocket *socket, const QString &response)
{
    // добавляем символ \n в конце сообщения и отправляем ответ клиенту, а так же принудительно сбрасываем буфер сокета
    socket->write((response + "\n").toUtf8());
    socket->flush();
    qDebug() << "Отправлено сообщение:" << response;
}

void ServerController::slotClientDisconnected()
{
    // определяем какой клиент отключился
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (clientSocket) {
        qDebug() << "Клиент отключился";
        // удаляем его буфер
        m_buffer.remove(clientSocket);
        // помечаем сокет на удаление
        clientSocket->deleteLater();
    }
}
