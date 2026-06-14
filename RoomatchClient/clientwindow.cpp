#include "clientwindow.h"
#include "networkmanager.h"
#include "ui_clientwindow.h"

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow) {
    ui->setupUi(this);

    initStackedWidget();
    initConnect();
    initNet();
}

ClientWindow::~ClientWindow() {
    delete ui;
}

void ClientWindow::initNet() {
    NetworkManager *netMgr = NetworkManager::instance();
    // UDP 广播写入在线教师下拉列表
    connect(netMgr, &NetworkManager::teacherDiscoveredUI, ui->loginPage, &LoginWidget::onTeacherDiscovered);
    connect(netMgr, &NetworkManager::teacherDismissedUI, ui->loginPage, &LoginWidget::onTeacherDismissed);

    // 给注册页面的下拉列表同样操作
    connect(netMgr, &NetworkManager::teacherDiscoveredUI, ui->registerPage, &StudentRegisterWidget::onTeacherDiscovered);
    connect(netMgr, &NetworkManager::teacherDismissedUI, ui->registerPage, &StudentRegisterWidget::onTeacherDismissed);

    netMgr->startService();
}

void ClientWindow::initConnect() {
    // 登录页面->注册页面
    connect(ui->loginPage, &LoginWidget::toRegisterPage, this, [this](){
        smoothChangePage(ui->loginPage, ui->registerPage);
    });

    // 注册页面->登录页面
    connect(ui->registerPage, &StudentRegisterWidget::toLoginPage, this, [this](){
        smoothChangePage(ui->registerPage, ui->loginPage);
    });
}

void ClientWindow::initStackedWidget() {
    // 测试专用
    // 默认 登录界面为首 0
    for (int i = 0; i < ui->stackedWidget->count(); i++) {
        QWidget *page = ui->stackedWidget->widget(i);
        page->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    }
    ui->loginPage->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
    this->adjustSize();
}

void ClientWindow::smoothChangePage(QWidget *OLD, QWidget *NEW) {
    OLD->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    NEW->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->stackedWidget->setCurrentWidget(NEW);
    this->adjustSize();
}

