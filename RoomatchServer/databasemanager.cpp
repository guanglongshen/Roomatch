#include "databasemanager.h"


DatabaseManager* DatabaseManager::instance() {
    static DatabaseManager inst;
    return &inst;
}

DatabaseManager::~DatabaseManager() {
    // 线程如果还在运行，那么阻塞等待，直到线程退出成功
    if (managerThread->isRunning()) {
        managerThread->quit();
        managerThread->wait();
    }
}

void DatabaseManager::startService() {
    if (!managerThread->isRunning()) {
        managerThread->start();
    }
}

DatabaseManager::DatabaseManager() {
    // 创建后台 worker 线程
    managerThread = new QThread(this);
    worker = new DatabaseWorker();
    worker->moveToThread(managerThread);

    // worker 与 manager 信号连接
    connect(managerThread, &QThread::started, worker, &DatabaseWorker::onInitializeDatabase);
    connect(managerThread, &QThread::finished, worker, &QObject::deleteLater);

    // worker 所有的 log 日志都会同步给 Manager
    connect(worker, &DatabaseWorker::logMessage, this, &DatabaseManager::logNotify);
    // worker 所有的 event 日志都会同步给 Manager
    connect(worker, &DatabaseWorker::eventMessage, this, &DatabaseManager::eventNotify);

    // 注册用户信号与槽
    connect(this, &DatabaseManager::registerRequest, worker, &DatabaseWorker::onRegisterUser);
    connect(worker, &DatabaseWorker::registerUserResponse, this, &DatabaseManager::registerResult);

    // 登录用户信号与槽
    connect(this, &DatabaseManager::loginRequest, worker, &DatabaseWorker::onLoginUser);
    connect(worker, &DatabaseWorker::loginUserResponse, this, &DatabaseManager::loginResult);

    // 学生注册
    connect(this, &DatabaseManager::registerStudentRequest, worker, &DatabaseWorker::onRegisterStudent);
    connect(worker, &DatabaseWorker::registerStudentResponse, this, &DatabaseManager::registerStudentResult);
}
