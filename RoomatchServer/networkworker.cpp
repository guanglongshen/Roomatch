#include "networkworker.h"

#include <protocols.h>

NetworkWorker::NetworkWorker(QObject *parent)
    : QObject{parent} {}

NetworkWorker::~NetworkWorker() {
    if (broadcastTimer) broadcastTimer->stop();
}

void NetworkWorker::onStartNetworkService() {
    // 初始化 UDP 套接字
    udpSocket = new QUdpSocket(this);

    // 初始化定时器 2s 刷新广播
    broadcastTimer = new QTimer(this);
    connect(broadcastTimer, &QTimer::timeout, this, &NetworkWorker::onSendBroadcast);
    broadcastTimer->start(2000);

    emit logMessage(tr("UDP 广播服务"), "ok", "Net Server");
}

void NetworkWorker::onSendBroadcast() {
    SERVERBROADCASTINFO info;

    // 填充魔数暗号
    memset(info.magic, 0, sizeof(info.magic));
    strncpy_s(info.magic, "ROOMATCH", 8);

    // 填充端口
    info.tcpPort = tcpPort;

    // 填充教师名
    memset(info.serverName, 0, sizeof(info.serverName));
    strncpy_s(info.serverName, "Default Teacher", 31);

    // 将结构体强转为 char*
    // 随后广播全网 IP(255.255.255.255)，学生端在听端口为 55521
    QByteArray datagram(reinterpret_cast<const char*>(&info), sizeof(info));
    udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, 55521);

    // 测试
    // emit logMessage("UDP 广播心跳包", "ok", "Net Server");
}
