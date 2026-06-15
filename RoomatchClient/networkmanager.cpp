#include "networkmanager.h"

NetworkManager::NetworkManager() : managerThread(nullptr), worker(nullptr) {
    managerThread = new QThread(this);
    worker = new NetworkWorker();
    worker->moveToThread(managerThread);

    // 生命周期控制
    connect(managerThread, &QThread::started, worker, &NetworkWorker::onInitializeNetwork);
    connect(managerThread, &QThread::finished, worker, &QObject::deleteLater);

    // 登录 / 注册 发送请求
    connect(this, &NetworkManager::loginRequest, worker, &NetworkWorker::onSendLoginRequest);
    connect(this, &NetworkManager::registerRequest, worker, &NetworkWorker::onSendRegisterRequest);

    connect(worker, &NetworkWorker::teacherDiscovered, this, &NetworkManager::teacherDiscoveredUI);
    connect(worker, &NetworkWorker::teacherDismissed, this, &NetworkManager::teacherDismissedUI);
    // 登录 / 注册 结果
    connect(worker, &NetworkWorker::loginResponse, this, &NetworkManager::loginReply);
    connect(worker, &NetworkWorker::registerResponse, this, &NetworkManager::registerReply);
}

NetworkManager::~NetworkManager() {
    if (managerThread) {
        managerThread->quit();
        managerThread->wait();
    }
}

NetworkManager *NetworkManager::instance() {
    static NetworkManager manager;
    return &manager;
}

void NetworkManager::startService() {
    if (!managerThread->isRunning()) {
        managerThread->start();
    }
}
