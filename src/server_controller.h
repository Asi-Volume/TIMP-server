/**
 * @file server_controller.h
 * @brief Контроллер TCP-сервера.
 * @details Управляет жизненным циклом сервера, принимает входящие
 * соединения, накапливает данные в буфере и передает в ServerModel для обработки.
 */

#ifndef SERVER_CONTROLLER_H
#define SERVER_CONTROLLER_H

#include <QMap>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "server_model.h"

/**
 * @class ServerController
 * @brief Класс для управления сетевым взаимодействием.
 */
class ServerController : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор контроллера.
     * @param parent Родительский объект Qt.
     */
    explicit ServerController(QObject *parent = nullptr);

    /**
     * @brief Деструктор (останавливает сервер и освобождает ресурсы).
     */
    ~ServerController();

    /**
     * @brief Запуск сервера на прослушивание порта.
     * @param port Порт, который будет открыт.
     * @return true при успешном запуске сервера.
     */
    bool startServer(quint16 port);

    /**
     * @brief Остановка сервера и закрытие клиентских соединений.
     */
    void stopServer();

private slots:
    /**
     * @brief Обработка нового входящего подключения.
     */
    void slotNewConnection();

    /**
     * @brief Чтение данных из сокета клиента.
     */
    void slotServerRead();

    /**
     * @brief Очистка ресурсов при отключении клиента.
     */
    void slotClientDisconnected();

private:
    /**
     * @brief Разбор сообщения и формирование ответа через модель.
     * @param message Строка запроса от клиента.
     * @param socket Указатель на сокет клиента для отправки ответа.
     */
    void processMessage(const QString &message, QTcpSocket *socket);

    /**
     * @brief Отправка строки ответа клиенту.
     * @param socket Сокет получателя.
     * @param response Текст ответа.
     */
    void sendResponse(QTcpSocket *socket, const QString &response);

    QTcpServer *m_tcpServer;                    ///< Объект TCP-сервера Qt
    QMap<QTcpSocket *, QString> m_buffer;       ///< Буферы для неполных сообщений от клиентов
    ServerModel *m_model;                       ///< Указатель на объект
};

#endif
