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
}

ServerWindow::~ServerWindow() {
    delete ui;
}
