#include "networkmanager.h"
#include "studenthomewidget.h"
#include "ui_studenthomewidget.h"

StudentHomeWidget::StudentHomeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StudentHomeWidget) {
    ui->setupUi(this);

    initConnection();
}

StudentHomeWidget::~StudentHomeWidget() {
    delete ui;
}

void StudentHomeWidget::initConnection() {
    // 退出登录按钮
    connect(ui->quitBtn, &QPushButton::clicked, this, [this](){
        emit toLoginPage();     // 返回登录页面

        // 发出信号，让 worker 去执行 onSendLogoutRequest
        emit NetworkManager::instance()->logoutRequest();
    });
}
