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
        NET_STATUS fail;
        fail.code = 300;
        strncpy_s(fail.msg, tr("网络连接失败！").toUtf8().constData(), sizeof(fail.msg) - 1);
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
    qDebug() << "准备发送";
    if (tcpClient->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "连接状态不对";
        tcpClient->disconnectFromHost();
    }

    tcpClient->connectToHost(ip, port);
    // 3s 连接宽容度
    if (!tcpClient->waitForConnected(3000)) {
        NET_STATUS fail;
        fail.code = 300;
        strncpy_s(fail.msg, tr("网络连接失败！").toUtf8().constData(), sizeof(fail.msg) - 1);

        emit registerResponse(fail);
        qDebug() << "连接超时";
        return ;
    }

    qDebug() << "连接ok";

    // 包头
    PACKETHEADER header;
    header.magic = MAGICNUM;
    header.type = MSG_REGISTER_REQ;
    header.length = sizeof(USERINFO_PACKET);

    // 包体
    USERINFO_PACKET packet;
    memset(&packet, 0, sizeof(USERINFO_PACKET));
    QByteArray userBytes = info.username.toUtf8();
    QByteArray pwdBytes = info.pwd.toUtf8();

    strncpy_s(packet.username, userBytes.constData(), sizeof(packet.username) - 1);
    strncpy_s(packet.password, pwdBytes.constData(), sizeof(packet.password) - 1);
    packet.type = (quint16)info.type;

    // 序列化
    QByteArray buffer;
    buffer.append(reinterpret_cast<char*>(&header), sizeof(PACKETHEADER));
    buffer.append(reinterpret_cast<char*>(&packet), sizeof(USERINFO_PACKET));

    tcpClient->write(buffer);
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

    qDebug() << "===[网络层]=== 收到 TCP 数据！当前缓冲区可用字节数:" << tcpClient->bytesAvailable();

    while (tcpClient->bytesAvailable() > 0) {
        if (tcpClient->bytesAvailable() < sizeof(PACKETHEADER)) {
            qDebug() << "⚠ 墙 1 放行失败：当前字节数不够一个包头大小(" << sizeof(PACKETHEADER) << ")，继续等待...";
            return ;
        }

        PACKETHEADER header;
        tcpClient->peek(reinterpret_cast<char*>(&header), sizeof(PACKETHEADER));

        qDebug() << "🔍 解析包头 -> Magic:" << header.magic
                 << " | Type:" << header.type
                 << " | Length:" << header.length;

        if (header.magic != MAGICNUM) {
            qDebug() << "❌ 墙 2 拦截：魔数不匹配！期望:" << MAGICNUM << " 实际收到:" << header.magic << "。正在强制断开！";
            tcpClient->disconnectFromHost();
            return ;
        }

        if ((quint64)tcpClient->bytesAvailable() < (quint64)(sizeof(PACKETHEADER) + header.length)) {
            qDebug() << "⚠ 墙 3 放行失败：数据未接收全。期望总大小:" << sizeof(PACKETHEADER) + header.length
                     << " 当前只有:" << tcpClient->bytesAvailable() << "，等待下一次 readyRead...";
            return ;
        }

        // 读取包体
        QByteArray allData = tcpClient->read(sizeof(PACKETHEADER) + header.length);
        QByteArray bodyData = allData.mid(sizeof(PACKETHEADER));

        // 拆解回复包
        if (header.type == MSG_REGISTER_RES) {
            // 注册信息发回
            if (bodyData.size() >= sizeof(NET_STATUS)) {
                NET_STATUS *resStatus = reinterpret_cast<NET_STATUS*>(bodyData.data());
                // qDebug() << "注册信息拿回";
                emit registerResponse(*resStatus);
            }

            tcpClient->disconnectFromHost();
        } else if (header.type == MSG_LOGIN_RES) {
            // 登录信息
            if (bodyData.size() >= sizeof(NET_STATUS)) {
                NET_STATUS *loginStatus = reinterpret_cast<NET_STATUS*>(bodyData.data());
                emit loginResponse(*loginStatus);
            }
        }
    }
}
