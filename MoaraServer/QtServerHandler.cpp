#include "QtServerHandler.h"
#include <QDebug>
#include "QtCore/QThread"
#include "QtWidgets/QApplication"

QtServerHandler::QtServerHandler(QObject* parent)
	: QObject(nullptr)
{
}

QtServerHandler::~QtServerHandler()
{
	if (m_webSocketServer)
	{
		m_webSocketServer->close();
		m_webSocketServer->deleteLater();
	}
}

bool QtServerHandler::StartServer(unsigned short port,
							DataReceivedCallback dataCallback,
							DisconnectedCallback disconnectedCallback)
{
	m_dataReceivedCallback = dataCallback;
	m_disconnectedCallback = disconnectedCallback;

	m_webSocketServer = new QWebSocketServer(QStringLiteral("Qt WebSocket Server"),
		QWebSocketServer::NonSecureMode,
		this);

	if (!m_webSocketServer->listen(QHostAddress::Any, port))
	{
		qWarning() << "Could not start server on port" << port
			<< "-" << m_webSocketServer->errorString();
		return false;
	}

	connect(m_webSocketServer, &QWebSocketServer::newConnection,
		this, &QtServerHandler::onNewConnection);

	qDebug() << "Server started. Listening on port:" << port;
	return true;
}

void QtServerHandler::SendData(const void* data, int dataSize, int clientId)
{
	QByteArray byteArray(static_cast<const char*>(data), dataSize);
	QString textMessage = QString::fromUtf8(byteArray);
	QWebSocket* clientSocket = m_clients[clientId];

	if (QThread::currentThread() == qApp->thread())
	{
		if (clientSocket)
		{
			clientSocket->sendTextMessage(textMessage);
		}
	}
	else
	{
		QMetaObject::invokeMethod(
			qApp,
			[clientSocket, textMessage]() {
				if (clientSocket)
				{
					clientSocket->sendTextMessage(textMessage);
				}
			},
			Qt::QueuedConnection);
	}

	qDebug() << "Sent message to clientId = " << clientId << " : " << textMessage;
}

void QtServerHandler::SetDataReceivedCallback(DataReceivedCallback callback)
{
	m_dataReceivedCallback = callback;
}

void QtServerHandler::SetDisconnectedCallback(DisconnectedCallback callback)
{
	m_disconnectedCallback = callback;
}

void QtServerHandler::onNewConnection()
{
	QWebSocket* clientSocket = m_webSocketServer->nextPendingConnection();
	if (!clientSocket)
		return;

	connect(clientSocket, &QWebSocket::textMessageReceived, this, &QtServerHandler::onTextMessageReceived);
	connect(clientSocket, &QWebSocket::disconnected, this, &QtServerHandler::onClientDisconnected);

	m_clients.push_back(clientSocket);
	int clientId = static_cast<int>(m_clients.size()) - 1;

	qDebug() << "New client connected with clientId = " << clientId;
}

void QtServerHandler::onTextMessageReceived(const QString& message)
{
	QWebSocket* client = qobject_cast<QWebSocket*>(sender());
	if (!client)
		return;

	int clientId = GetClientIndex(client);

	qDebug() << "Received message from clientId = " << clientId << " : " << message;

	QByteArray data = message.toUtf8();
	m_dataReceivedCallback(data.constData(), data.size(), clientId);
}

void QtServerHandler::onClientDisconnected()
{
	QWebSocket* client = qobject_cast<QWebSocket*>(sender());
	if (!client)
		return;

	int clientId = GetClientIndex(client);
	qDebug() << "Client disconnected";

	m_disconnectedCallback(clientId);

	client->deleteLater();

	if (clientId >= 0 && clientId < static_cast<int>(m_clients.size()))
	{
		//m_clients[clientId] = nullptr;
		m_clients.erase(m_clients.begin() + clientId);
	}
}

int QtServerHandler::GetClientIndex(QWebSocket* client)
{
	for (int i = 0; i < static_cast<int>(m_clients.size()); i++)
	{
		if (m_clients[i] == client)
			return i;
	}
	return -1;
}
