#pragma once

#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <string>

class LoginManager : public QFrame
{
	Q_OBJECT

public:
	LoginManager(QWidget* parent);
	~LoginManager();

public slots:
	void showEvent(QShowEvent* event) override;

signals:
	void GoToSignUpScene();
	void GoToLobby();

private slots:
	void OnLogInCredentialsSent() noexcept;
	void OnTogglePasswordView() noexcept;

private:
	bool m_firstShow;
	QLineEdit* m_nameInput;
	QLineEdit* m_passwordInput;

	QPushButton* m_logInButton;
	QPushButton* m_togglePasswordView;
	QPushButton* m_signupButton;

	QIcon m_viewPasswordIcon;
	QIcon m_hidePasswordIcon;

	QLabel* m_incorrectCredentialsLabel{ new QLabel{} };
	QString m_incorrectCredentialsText{ "Incorrect username or password. Please try again." };
};
