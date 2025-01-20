#pragma once

#include <QFrame>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

class SignupManager : public QFrame
{
	Q_OBJECT

public:
	SignupManager(QWidget* parent = nullptr);
	~SignupManager() = default;

public slots:
	void showEvent(QShowEvent* event) override;

private:
	void OnSignUpCredentialsSent() noexcept;
	void OnTogglePasswordView() noexcept;

signals:
	void GoToLogIn();

private:
	bool m_firstShow;
	QLineEdit* m_nameInput;
	QLineEdit* m_passwordInput;

	QPushButton* m_signUpButton;
	QPushButton* m_togglePasswordView;
	QPushButton* m_goToLogInButton;

	QLabel* m_alreadyExistingAccountLabel;
	QString m_incorrectCredentialsText;

	QIcon m_viewPasswordIcon;
	QIcon m_hidePasswordIcon;
};
