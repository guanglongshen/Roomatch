#include "networkmanager.h"

NetworkManager::NetworkManager() {
    netThread = new QThread(this);
    worker = new NetworkWorker();
    worker->moveToThread(netThread);

    connect(netThread, &QThread::started, worker, &NetworkWorker::onStartNetworkService);
    connect(worker, &NetworkWorker::logMessage, this, &NetworkManager::logNotify);
    connect(netThread, &QThread::finished, worker, &QObject::deleteLater);
}

NetworkManager *NetworkManager::instance() {
    static NetworkManager manager;
    return &manager;
}

// 析构函数 关闭线程
NetworkManager::~NetworkManager() {
    if (netThread) {
        netThread->quit();
        netThread->wait();
    }
}

// 启动服务
void NetworkManager::startService() {
    if (!netThread->isRunning()) {
        netThread->start();
    }
}


