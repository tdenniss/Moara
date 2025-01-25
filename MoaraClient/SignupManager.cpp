#include "SignupManager.h"

SignupManager::SignupManager(QWidget* parent)
	: QFrame{ parent }
	, m_firstShow { true }
	, m_nameInput{ new QLineEdit{} }
	, m_passwordInput{ new QLineEdit{} }
	, m_signUpButton{ new QPushButton{} }
	, m_togglePasswordView{ new QPushButton{} }
	, m_alreadyExistingAccountLabel{ new QLabel{} }
{
	m_viewPasswordIcon = QIcon(QPixmap(":/others/view_password"));
	m_hidePasswordIcon = QIcon(QPixmap(":/others/hide_password"));
	m_incorrectCredentialsText = "Credentials match existing account.";
}

void SignupManager::OnSignUpSuccess()
{

	emit GoToLogIn(m_sdk);
}

void SignupManager::OnError(const std::string& message)
{

}

void SignupManager::OnGoToSignUp(IClientSDKPtr sdk)
{
	m_sdk = sdk;
}

void SignupManager::showEvent(QShowEvent* event) {
	if (m_firstShow) {
		m_nameInput = findChild<QLineEdit*>("enterNameSignUpInput");
		m_passwordInput = findChild<QLineEdit*>("enterPasswordSignUpInput");
		m_signUpButton = findChild<QPushButton*>("signUpButton");
		m_goToLogInButton = findChild<QPushButton*>("goToLogInFromSignUpButton");
		m_alreadyExistingAccountLabel = findChild<QLabel*>("alreadyExistingAccountLabel");
		m_togglePasswordView = findChild<QPushButton*>("toggleSignUpPasswordViewButton");

		QObject::connect(m_signUpButton, &QPushButton::released, this, &SignupManager::OnSignUpCredentialsSent);
		QObject::connect(m_togglePasswordView, &QPushButton::released, this, &SignupManager::OnTogglePasswordView);
		QObject::connect(m_goToLogInButton, &QPushButton::released, this, [this]() {
			emit GoToLogIn(m_sdk);
		});

		m_firstShow = false;
	}
	m_alreadyExistingAccountLabel->clear();

	m_sdk->SetListener(this);
}

void SignupManager::OnSignUpCredentialsSent() noexcept
{
	m_sdk->SignUp(m_nameInput->text().toStdString(), m_passwordInput->text().toStdString());
}

void SignupManager::OnTogglePasswordView() noexcept
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
