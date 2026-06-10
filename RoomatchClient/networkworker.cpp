#include "networkworker.h"
#include "protocols.h"

NetworkWorker::NetworkWorker(QObject *parent)
    : QObject{parent} {}

void NetworkWorker::onInitializeNetwork() {
    udpListener = new QUdpSocket(this);
    tcpClient = new QTcpSocket(this);

    // 绑定局域网大喇叭端口
    bool bindOk = udpListener->bind(studentListenerPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    if (bindOk) {
        connect(udpListener, &QUdpSocket::readyRead, this, &NetworkWorker::onReadPendingDatagrams);
    } else {
        qDebug() << "bind failed.";
        return ;
    }

    // 绑定 TCP 信号
    connect(tcpClient, &QTcpSocket::readyRead, this, &NetworkWorker::onTcpReadyRead);
}

// 发送登录请求
void NetworkWorker::onSendLoginRequest(const QString &ip, quint16 port, const USERINFO &info) {
    if (tcpClient->state() == QAbstractSocket::ConnectedState) {
        tcpClient->disconnectFromHost();
    }

    // 异步链接
    tcpClient->connectToHost(ip, port);
    // 3s 连接宽容度
    if (!tcpClient->waitForConnected(3000)) {
        STATUS fail = { 300, tr("网络连接失败！") };
        emit loginResponse(fail);
        return ;
    }

    // 包头
    PACKETHEADER header;
    header.magic = MAGICNUM;
    header.type = MSG_LOGIN_REQ;
    header.length = sizeof(USERINFO_PACKET);

    // 包体
    USERINFO_PACKET packet;
    QByteArray userBytes = info.username.toUtf8();
    QByteArray pwdBytes = info.pwd.toUtf8();

    strncpy_s(packet.username, userBytes.constData(), sizeof(packet.username) - 1);
    strncpy_s(packet.password, pwdBytes.constData(), sizeof(packet.password) - 1);
    packet.type = (quint16)info.type;

    // 打包数据
    QByteArray buffer;
    buffer.append(reinterpret_cast<char*>(&header), sizeof(PACKETHEADER));
    buffer.append(reinterpret_cast<char*>(&packet), sizeof(USERINFO_PACKET));

    // 写入网卡
    tcpClient->write(buffer);
}

void NetworkWorker::onSendRegisterRequest(const QString &ip, quint16 port, const USERINFO &info) {
    // TODO：注册请求
}

// UDP 接收
void NetworkWorker::onReadPendingDatagrams() {
    while (udpListener->hasPendingDatagrams()) {
        // 接收数据的大小
        QByteArray datagram;
        datagram.resize(udpListener->pendingDatagramSize());

        // 发送者 IP
        QHostAddress senderIP;
        udpListener->readDatagram(datagram.data(), datagram.size(), &senderIP);

        // UDP 数据未获取完全
        if (datagram.size() < sizeof(SERVERBROADCASTINFO)) {
            continue;
        }

        SERVERBROADCASTINFO *info = reinterpret_cast<SERVERBROADCASTINFO*>(datagram.data());
        if (strncmp(info->magic, "ROOMATCH", 8) == 0) {
            QString teacherName = QString::fromUtf8(info->serverName);
            QString ipStr = senderIP.toString().remove("::ffff:");

            // 离线包
            if (info->state == BROADCAST_OFFLINE) {
                emit teacherDismissed(teacherName, ipStr, info->tcpPort);
            } else {
                emit teacherDiscovered(teacherName, ipStr, info->tcpPort);
            }
        }
    }
}

void NetworkWorker::onTcpReadyRead() {
    // 处理教师端发回来的 结果包
    // 剥离数据，发信号处理 UI
}
