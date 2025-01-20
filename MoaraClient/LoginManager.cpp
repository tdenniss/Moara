#include "LogInManager.h"
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
	m_viewPasswordIcon = QIcon(QPixmap(":/others/view_password"));
	m_hidePasswordIcon = QIcon(QPixmap(":/others/hide_password"));
	m_incorrectCredentialsText = "Incorrect username or password. Please try again.";
}

LoginManager::~LoginManager()
{
}

void LoginManager::showEvent(QShowEvent* event) {
	if (m_firstShow) {
		m_nameInput = findChild<QLineEdit*>("enterNameLogInInput");
		m_passwordInput = findChild<QLineEdit*>("enterPasswordLogInInput");
		m_logInButton = findChild<QPushButton*>("logInButton");
		m_incorrectCredentialsLabel = findChild<QLabel*>("incorrectCredentialsLabel");
		m_togglePasswordView = findChild<QPushButton*>("toggleLogInPasswordViewButton");

		QObject::connect(m_logInButton, &QPushButton::released, this, &LoginManager::OnLogInCredentialsSent);
		QObject::connect(m_togglePasswordView, &QPushButton::released, this, &LoginManager::OnTogglePasswordView);
		QObject::connect(m_signupButton, &QPushButton::released, this, [this]() {
			emit GoToSignUpScene();
		});

		m_firstShow = false;
	}
	m_incorrectCredentialsLabel->clear();
}

void LoginManager::OnLogInCredentialsSent() noexcept
{
	
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
