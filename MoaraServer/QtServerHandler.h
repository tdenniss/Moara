#pragma once

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>

#include <vector>

#include "IServerHandler.h"

class QtServerHandler : public QObject, public IServerHandler
{
	Q_OBJECT
public:
	explicit QtServerHandler(QObject* parent = nullptr);
	~QtServerHandler();

	bool StartServer(unsigned short port,
					DataReceivedCallback dataCallback,
					DisconnectedCallback disconnectedCallback) override;

	void SendData(const void* data, int dataSize, int clientId) override;

	void SetDataReceivedCallback(DataReceivedCallback callback) override;
	void SetDisconnectedCallback(DisconnectedCallback callback) override;

private slots:
	void onNewConnection();
	void onTextMessageReceived(const QString& message);
	void onClientDisconnected();

private:
	int GetClientIndex(QWebSocket* client);

private:
	QWebSocketServer* m_webSocketServer{ nullptr };
	std::vector<QWebSocket*> m_clients;

	DataReceivedCallback m_dataReceivedCallback;
	DisconnectedCallback m_disconnectedCallback;
};
