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

    // 设置 Action
    connect(ui->actionLogKeeper, &QAction::triggered, this, [this](){
        if (logKeeper == nullptr) {
            logKeeper = new LogKeeper(this);

            // // 通过 WindowFlags，使其独立弹窗，并绑定主窗口 ServerWindow，共同销毁
            // logKeeper->setWindowFlags(Qt::Window);
            // logKeeper->setParent(this, Qt::Window);

            // 关闭时为隐藏
            logKeeper->setAttribute(Qt::WA_DeleteOnClose, false);

            connect(DatabaseManager::instance(), &DatabaseManager::logNotify, logKeeper, &LogKeeper::addSystemLog);
        }

        logKeeper->show();
        logKeeper->raise();
        logKeeper->activateWindow();
    });
}

ServerWindow::~ServerWindow() {
    delete ui;
}
