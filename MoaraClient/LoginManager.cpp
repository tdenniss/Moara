#include "LoginManager.h"
#include <iomanip>

LoginManager::LoginManager(QWidget* parent)
	: QFrame{ parent }
	, m_firstShow{ true }
	, m_nameInput{ new QLineEdit{} }
	, m_passwordInput{ new QLineEdit{} }
	, m_logInButton{ new QPushButton{} }
	, m_togglePasswordView{ new QPushButton{} }
	, m_signupButton{ new QPushButton{} }
	, m_incorrectCredentialsLabel{ new QLabel{} }
{
	m_sdk = IClientSDK::Create(SERVER_ADDRESS, PORT, HandlerType::QtWebSockets);
	m_sdk->SetListener(this);

	m_viewPasswordIcon = QIcon(QPixmap(":/others/view_password"));
	m_hidePasswordIcon = QIcon(QPixmap(":/others/hide_password"));
}

LoginManager::~LoginManager()
{
}

void LoginManager::OnLoginSuccess()
{
	emit GoToMenu(m_sdk);
}

void LoginManager::OnError(const std::string& message)
{
	m_incorrectCredentialsLabel->setText(QString::fromStdString(message));
	m_incorrectCredentialsLabel->show();
}

void LoginManager::OnGoToLogin(IClientSDKPtr sdk)
{
	m_sdk = sdk;
}

void LoginManager::showEvent(QShowEvent* event) {
	if (m_firstShow) {
		m_nameInput = findChild<QLineEdit*>("enterNameLogInInput");
		m_passwordInput = findChild<QLineEdit*>("enterPasswordLogInInput");
		m_logInButton = findChild<QPushButton*>("logInButton");
		m_signupButton = findChild<QPushButton*>("goToSignUpFromLogInButton");
		m_incorrectCredentialsLabel = findChild<QLabel*>("incorrectCredentialsLabel");
		m_togglePasswordView = findChild<QPushButton*>("toggleLogInPasswordViewButton");

		QObject::connect(m_logInButton, &QPushButton::released, this, &LoginManager::OnLogInCredentialsSent);
		QObject::connect(m_togglePasswordView, &QPushButton::released, this, &LoginManager::OnTogglePasswordView);
		QObject::connect(m_signupButton, &QPushButton::released, this, [this]() {
			emit GoToSignUpScene(m_sdk);
		});

		m_firstShow = false;
	}
	m_incorrectCredentialsLabel->clear();

	m_sdk->SetListener(this);
}

void LoginManager::OnLogInCredentialsSent() noexcept
{
	m_sdk->Login(m_nameInput->text().toStdString(), m_passwordInput->text().toStdString());
}

void LoginManager::OnTogglePasswordView() noexcept
{
	if (m_passwordInput->echoMode() == QLineEdit::Password) {
		m_togglePasswordView->setIcon(m_hidePasswordIcon);
		m_passwordInput->setEchoMode(QLineEdit::Normal);
	}
	else {
		m_togglePasswordView->setIcon(m_viewPasswordIcon);
		m_passwordInput->setEchoMode(QLineEdit::Password);
	}
}
