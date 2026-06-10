#include "networkmanager.h"

NetworkManager::NetworkManager() {
    netThread = new QThread(this);
    worker = new NetworkWorker();
    worker->moveToThread(netThread);

    connect(netThread, &QThread::started, worker, &NetworkWorker::onStartNetworkService);
    connect(worker, &NetworkWorker::logMessage, this, &NetworkManager::logNotify);
    connect(worker, &NetworkWorker::eventMessage, this, &NetworkManager::eventNotify);

    connect(netThread, &QThread::finished, worker, &QObject::deleteLater);
}

NetworkManager *NetworkManager::instance() {
    static NetworkManager manager;
    return &manager;
}

// 析构函数 关闭线程
NetworkManager::~NetworkManager() {
    if (netThread && netThread->isRunning()) {
        // 断开 socket
        QMetaObject::invokeMethod(worker, "onSendLogoutBroadcast", Qt::BlockingQueuedConnection);

        netThread->quit();
        netThread->wait();
    }

    if (worker && worker->thread() == QThread::currentThread()) {
        delete worker;
        worker = nullptr;
    }
}

void NetworkManager::setTeacherName(const QString &name) {
    worker->setServerName(name);
}

// 启动服务
void NetworkManager::startService() {
    if (!netThread->isRunning()) {
        netThread->start();
    } else {
        // 跨线程调用 worker 启动函数
        QMetaObject::invokeMethod(worker, "onStartNetworkService", Qt::BlockingQueuedConnection);
    }
}

// 关闭服务，登出状态 或 关闭软件
void NetworkManager::stopService() {
    if (netThread && netThread->isRunning()) {
        // 跨线程调用 worker 登出函数
        QMetaObject::invokeMethod(worker, "onSendLogoutBroadcast", Qt::BlockingQueuedConnection);
    }
}




