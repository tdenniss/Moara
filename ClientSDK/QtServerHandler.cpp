#include "QtServerHandler.h"

#include <QUrl>
#include <QDebug>

QtServerHandler::QtServerHandler()
	: QObject(nullptr)
	, m_isConnected(false)
	, m_newDataFunction(nullptr)
	, m_disconnectedFunction(nullptr)
{
	connect(&m_webSocket, &QWebSocket::connected,
		this, &QtServerHandler::onConnected);

	connect(&m_webSocket, &QWebSocket::disconnected,
		this, &QtServerHandler::onDisconnected);

	connect(&m_webSocket, &QWebSocket::textMessageReceived,
		this, &QtServerHandler::onTextMessageReceived);

	connect(&m_webSocket,
		QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred),
		this,
		&QtServerHandler::onErrorOccurred);
}

QtServerHandler::~QtServerHandler()
{
	Stop();
}

bool QtServerHandler::Start(const std::string& host, int port)
{
	if (m_isConnected)
	{
		return true;
	}

	QUrl url;
	url.setScheme("ws");
	url.setHost(QString::fromStdString(host));
	url.setPort(port);

	QString s = url.toString();
	qDebug() << "Connecting to server at" << s;

	m_webSocket.open(url);

	return true;
}

void QtServerHandler::Stop()
{
	if (m_isConnected)
	{
		qDebug() << "Closing websocket connection";
		m_webSocket.close();
		m_isConnected = false;
	}
}

void QtServerHandler::SendRequest(const void* message, int size)
{
	if (!m_isConnected)
		return;

	QString textMessage = QString::fromUtf8(static_cast<const char*>(message), size);

	m_webSocket.sendTextMessage(textMessage);
}

void QtServerHandler::SetNewDataCallback(NewDataCallback newDataFunction)
{
	m_newDataFunction = newDataFunction;
}

void QtServerHandler::SetDisconnectedCallback(DisconnectedCallback newDisconnectedFunction)
{
	m_disconnectedFunction = newDisconnectedFunction;
}

void QtServerHandler::onConnected()
{
	m_isConnected = true;
	qDebug() << "WebSocket connected!";
}

void QtServerHandler::onDisconnected()
{
	m_isConnected = false;
	qDebug() << "WebSocket disconnected";

	if (m_disconnectedFunction)
		m_disconnectedFunction();
}

void QtServerHandler::onTextMessageReceived(const QString& message)
{
	if (m_newDataFunction)
	{
		try
		{
			m_newDataFunction((void*)message.toStdString().c_str());
		}
		catch (const std::exception& e)
		{

		}
	}
}

void QtServerHandler::onErrorOccurred(QAbstractSocket::SocketError error)
{
	qDebug() << "WebSocket error occurred:" << error
		<< m_webSocket.errorString();
}
