#include "MoaraClient.h"

MoaraClient::MoaraClient(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    SetupConnections();
}

void MoaraClient::SetupConnections()
{
	//Log in Scene
	QObject::connect(ui.logInViewFrame, &LoginManager::GoToMenu, [this](IClientSDKPtr sdk) {
		ui.menuScene->OnGoToMenu(sdk);
		ui.stackedWidget->setCurrentWidget(ui.menuScene);
	});
	QObject::connect(ui.logInViewFrame, &LoginManager::GoToSignUpScene, [this](IClientSDKPtr sdk) {
		ui.signUpViewFrame->OnGoToSignUp(sdk);
		ui.stackedWidget->setCurrentWidget(ui.signupScene);
	});

	//Sign up Scene
	QObject::connect(ui.signUpViewFrame, &SignupManager::GoToLogIn, [this](IClientSDKPtr sdk) {
		ui.logInViewFrame->OnGoToLogin(sdk);
		ui.stackedWidget->setCurrentWidget(ui.loginScene);
	});

	//Menu scene
	QObject::connect(ui.menuScene, &MenuScene::SceneChange, [this](IClientSDKPtr sdk, EBoardType type, EBoardSize size, NodesInfo nodeInfoList) {
		ui.gameScene->OnSceneChange(sdk, type, size, nodeInfoList);
		ui.stackedWidget->setCurrentWidget(ui.gameScene);
	});

	//GameScene
	QObject::connect(ui.gameScene, &GameScene::LeaveGame, [this]() {
		ui.stackedWidget->setCurrentWidget(ui.menuScene);
	});

}
