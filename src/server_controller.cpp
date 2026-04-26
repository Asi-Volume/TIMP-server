/**
 * @file server_controller.cpp
 * @brief Реализация сетевой логики и управления подключениями.
 */

#include "server_controller.h"
#include <QDebug>

ServerController::ServerController(QObject *parent)
    : QObject(parent)
{
    // Создание TCP-сервера и модели, которая будет обрабатывать логику запросов
    m_tcpServer = new QTcpServer(this);
    m_model = new ServerModel();
}

ServerController::~ServerController()
{
    //  Остановка сервера и освобождение памяти при уничтожении контролерра
    stopServer();
    delete m_model;
}

bool ServerController::startServer(quint16 port)
{
    // Подключение сигнала нового входящего соединения к слоту обработки подключения
    connect(m_tcpServer, &QTcpServer::newConnection, this, &ServerController::slotNewConnection);

    // Запуск сервера на указанном порту
    if (!m_tcpServer->listen(QHostAddress::Any, port)) {
        qDebug() << "Не удалось запустить сервер на порту" << port;
        return false;
    }

    qDebug() << "Сервер запущен на" << port;
    return true;
}

void ServerController::stopServer()
{
    // Проверка работы сервера
    if (m_tcpServer->isListening()) {
        // Закрытие всех клиентских сокетов
        for (QTcpSocket *socket : m_buffer.keys()) {
            socket->close();
        }
        // Очистка буфера сообщений и выключение сервера
        m_buffer.clear();
        m_tcpServer->close();
        qDebug() << "Сервер прекратил работу";
    }
}

void ServerController::slotNewConnection()
{
    // Получение сокета нового клиента
    QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();
    qDebug() << "Новое подключение от" << clientSocket->peerAddress().toString();

    // Подключение сигнала readyRead, он сработает когда прийдут данные
    connect(clientSocket, &QTcpSocket::readyRead, this, &ServerController::slotServerRead);
    // Подключение сигнала отключения клиента
    connect(clientSocket,
            &QTcpSocket::disconnected,
            this,
            &ServerController::slotClientDisconnected);

    // Создание пустого буфера для нового клиента
    m_buffer[clientSocket] = "";
}

void ServerController::slotServerRead()
{
    // Определение сокета, от которого пришел сигнал readyRead
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket)
        return;

    // Считывание пришедших данных и добавление в буфер этого клиента
    QByteArray data = clientSocket->readAll();
    m_buffer[clientSocket] += QString::fromUtf8(data);

    // Если в буфере есть символ конца строки, значит сообщение пришло полностью
    if (m_buffer[clientSocket].contains('\n')) {
        // Разбиение буфера на отдельные сообщения
        QStringList messages = m_buffer[clientSocket].split('\n', Qt::SkipEmptyParts);
        // Очистка буфера после разбиения
        m_buffer[clientSocket] = "";

        // Обработка каждого сообщения отдельно
        for (const QString &msg : messages) {
            processMessage(msg.trimmed(), clientSocket);
        }
    }
}

void ServerController::processMessage(const QString &message, QTcpSocket *socket)
{
    qDebug() << "Получено сообщение:" << message;

    // Определение типа запроса
    RequestType type = m_model->parseRequest(message);
    // Переменная для ответа клиенту
    QString response;

    switch (type) {
    case RequestType::AUTH: {
        // Формат "auth&login&password"
        QStringList parts = message.split('&');
        if (parts.size() >= 3) {
            response = m_model->processAuth(parts[1], parts[2]);
        } else {
            response = "auth-";
        }
        break;
    }
    case RequestType::REG: {
        // Формат "reg&login&password&email"
        QStringList parts = message.split('&');
        if (parts.size() >= 4) {
            response = m_model->processReg(parts[1], parts[2], parts[3]);
        } else {
            response = "reg-";
        }
        break;
    }
    case RequestType::RECOVER_CODE: {
        // Формат "recover_code&email"
        QStringList parts = message.split('&');
        response = (parts.size() >= 2) ? m_model->processRecoverRequest(parts[1]) : "recover_code-";
        break;
    }
    case RequestType::RECOVER_CONF: {
        // Формат "recover_confirm&email&code&new_password"
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


    // Отправка сформированного ответа клиенту
    sendResponse(socket, response);
}

void ServerController::sendResponse(QTcpSocket *socket, const QString &response)
{
    // Добавление символа \n в конце сообщения, отправка ответа клиенту, принудительный сброс буфера сокета
    socket->write((response + "\n").toUtf8());
    socket->flush();
    qDebug() << "Отправлено сообщение:" << response;
}

void ServerController::slotClientDisconnected()
{
    // Идентификация отключенного клиента
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (clientSocket) {
        qDebug() << "Клиент отключился";
        // Удаление буфера
        m_buffer.remove(clientSocket);
        // Пометка сокета для удаления
        clientSocket->deleteLater();
    }
}
