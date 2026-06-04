#include "serverwindow.h"
#include "ui_serverwindow.h"
#include "databasemanager.h"

#include <QStatusBar>

ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerWindow) {
    ui->setupUi(this);

    // 后台数据库链接
    DatabaseManager *dbMgr = DatabaseManager::instance();
    connect(dbMgr, &DatabaseManager::logNotify, this, [this](const QString &msg){
        this->ui->statusbar->showMessage(msg, 1000);
    });

    // 在连接信号后，启动线程
    dbMgr->startService();

    // 与注册页面连接
    connect(ui->LoginPage, &LoginWidget::toRegisterPage, this, [this](){
        ui->stackedWidget->setCurrentIndex(1);  // 切换到 1-注册页
    });

    // 注册页面返回到登录页面
    connect(ui->RegisterPage, &RegisterWidget::toLoginPage, this, [this](){
        ui->stackedWidget->setCurrentIndex(0);  // 切换到 0-登录页
    });
}

ServerWindow::~ServerWindow() {
    delete ui;
}
