#pragma once

#include "IServerHandler.h"

#include <QObject>
#include <QWebSocket>

class QWebSocket;

class QtServerHandler : public QObject, public IServerHandler
{
	Q_OBJECT
public:
	QtServerHandler();
	~QtServerHandler() override;

	// IServerHandler interface
	bool Start(const std::string& host, int port) override;
	void Stop() override;

	void SendRequest(const void* message, int size) override;

	void SetNewDataCallback(NewDataCallback newDataFunction) override;
	void SetDisconnectedCallback(DisconnectedCallback newDisconnectedFunction) override;

private slots:
	void onConnected();
	void onDisconnected();
	void onTextMessageReceived(const QString& message);
	void onErrorOccurred(QAbstractSocket::SocketError error);

private:
	QWebSocket m_webSocket;
	bool m_isConnected;

	NewDataCallback m_newDataFunction;
	DisconnectedCallback m_disconnectedFunction;
};
