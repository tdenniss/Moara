#include "MoaraClient.h"

MoaraClient::MoaraClient(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    SetupConnections();
}

void MoaraClient::SetupConnections()
{
	QObject::connect(ui.menuScene, &MenuScene::SceneChange, [this](IClientSDKPtr sdk, EBoardType type, EBoardSize size, NodesInfo nodeInfoList) {
		ui.gameScene->OnSceneChange(sdk, type, size, nodeInfoList);
		ui.stackedWidget->setCurrentWidget(ui.gameScene);
	});

}
