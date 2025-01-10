#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MoaraClient.h"

class MoaraClient : public QMainWindow
{
    Q_OBJECT

public:
    MoaraClient(QWidget *parent = nullptr);
    ~MoaraClient();

private:
    Ui::MoaraClientClass ui;
};
