#include "serverwindow.h"
#include "ui_serverwindow.h"
#include "databasemanager.h"

#include "networkmanager.h"

#include <QMessageBox>
#include <QStatusBar>

ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerWindow) {
    ui->setupUi(this);
    initStackedWidget();

    initDatabase();
    initConnection();
    initNet();
}

ServerWindow::~ServerWindow() {
    NetworkManager::instance()->stopService();
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
    connect(dbMgr, &DatabaseManager::eventNotify, logKeeper, &LogKeeper::addEventLog);

    // 在连接信号后，启动线程
    dbMgr->startService();
}

void ServerWindow::initConnection() {
    // 与注册页面连接
    connect(ui->LoginPage, &LoginWidget::toRegisterPage, this, [this](){
        smoothChangePage(ui->LoginPage, ui->RegisterPage);
    });

    // 注册页面返回到登录页面
    connect(ui->RegisterPage, &RegisterWidget::toLoginPage, this, [this](){
        smoothChangePage(ui->RegisterPage, ui->LoginPage);
    });

    // 登录消息接收
    connect(DatabaseManager::instance(), &DatabaseManager::loginResult, this, [this](const STATUS &status, const QString &username){
        // 解除登录页面的登录按钮
        ui->LoginPage->setLoginEnableBtn(true);

        if (status.code == 0) {
            // 登录成功，跳转至 HomePage
            smoothChangePage(ui->LoginPage, ui->HomePage);
            ui->HomePage->setTeacher(username);
            this->statusBar()->showMessage(tr("%1, 欢迎登录 Room Match 平台！").arg(username), 2000);

            NetworkManager::instance()->setTeacherName(username);
            NetworkManager::instance()->startService();

        } else {
            QMessageBox::information(this, "登录消息", status.info + "\t\t");
        }
    });

    // 主页面退出登录
    connect(ui->HomePage, &HomeWidget::quitLogin, this, [this](){
        smoothChangePage(ui->HomePage, ui->LoginPage);
        this->statusBar()->showMessage(tr("已成功退出登录！"), 2000);

        // 网络停止服务
        NetworkManager::instance()->stopService();
    });

    // 设置 Action
    connect(ui->actionLogKeeper, &QAction::triggered, this, [this](){
        if (logKeeper == nullptr) {
            logKeeper = new LogKeeper(this);

            // 关闭时为隐藏
            logKeeper->setAttribute(Qt::WA_DeleteOnClose, false);
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
    connect(netMgr, &NetworkManager::eventNotify, logKeeper, &LogKeeper::addEventLog);
}

void ServerWindow::initStackedWidget() {
    // 测试专用
    // 默认 登录界面为首 0
    for (int i = 0; i < ui->stackedWidget->count(); i++) {
        QWidget *page = ui->stackedWidget->widget(i);
        page->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    }
    ui->LoginPage->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->stackedWidget->setCurrentWidget(ui->LoginPage);
    this->adjustSize();
}

void ServerWindow::smoothChangePage(QWidget *OLD, QWidget *NEW) {
    OLD->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    NEW->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->stackedWidget->setCurrentWidget(NEW);
    this->adjustSize();
}
