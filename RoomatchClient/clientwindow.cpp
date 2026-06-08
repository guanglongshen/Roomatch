#include "clientwindow.h"
#include "networkmanager.h"
#include "ui_clientwindow.h"

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow) {
    ui->setupUi(this);

    initNet();
}

ClientWindow::~ClientWindow() {
    delete ui;
}

void ClientWindow::initNet() {
    qDebug() << "init...";
    NetworkManager *netMgr = NetworkManager::instance();
    connect(netMgr, &NetworkManager::teacherDiscoveredUI, ui->loginPage, &LoginWidget::onTeacherDiscovered);
    netMgr->startService();
}

