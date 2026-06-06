#include "serverwindow.h"
#include "ui_serverwindow.h"
#include "databasemanager.h"

#include "networkmanager.h"

#include <QStatusBar>

ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerWindow) {
    ui->setupUi(this);
    // 登录界面为首
    ui->stackedWidget->setCurrentIndex(0);

    initDatabase();
    initConnection();
    initNet();
}

ServerWindow::~ServerWindow() {
    delete ui;
}

void ServerWindow::initDatabase() {
    // 后台数据库链接
    DatabaseManager *dbMgr = DatabaseManager::instance();

    // 结合 LogKeeper
    logKeeper = new LogKeeper(this);

    // 关闭时为隐藏
    logKeeper->setAttribute(Qt::WA_DeleteOnClose, false);
    connect(dbMgr, &DatabaseManager::logNotify, logKeeper, &LogKeeper::addSystemLog);

    // 在连接信号后，启动线程
    dbMgr->startService();
}

void ServerWindow::initConnection() {
    // 与注册页面连接
    connect(ui->LoginPage, &LoginWidget::toRegisterPage, this, [this](){
        ui->stackedWidget->setCurrentIndex(1);  // 切换到 1-注册页
    });

    // 注册页面返回到登录页面
    connect(ui->RegisterPage, &RegisterWidget::toLoginPage, this, [this](){
        ui->stackedWidget->setCurrentIndex(0);  // 切换到 0-登录页
    });

    // 设置 Action
    connect(ui->actionLogKeeper, &QAction::triggered, this, [this](){
        if (logKeeper == nullptr) {
            logKeeper = new LogKeeper(this);

            // 关闭时为隐藏
            logKeeper->setAttribute(Qt::WA_DeleteOnClose, false);
            connect(DatabaseManager::instance(), &DatabaseManager::logNotify, logKeeper, &LogKeeper::addSystemLog);
        }

        logKeeper->show();
        logKeeper->raise();
        logKeeper->activateWindow();
    });
}

void ServerWindow::initNet() {
    // 网络测试
    NetworkManager *netMgr = NetworkManager::instance();
    connect(netMgr, &NetworkManager::logNotify, logKeeper, &LogKeeper::addSystemLog);
    netMgr->startService();
}
