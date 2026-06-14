#include "networkworker.h"

#include <protocols.h>

NetworkWorker::NetworkWorker(QObject *parent)
    : QObject{parent} {}

NetworkWorker::~NetworkWorker() {
    if (broadcastTimer) broadcastTimer->stop();
}

void NetworkWorker::onStartNetworkService() {
    // 初始化 UDP 套接字
    if (udpSocket == nullptr) udpSocket = new QUdpSocket(this);
    // 测试
    emit logMessage(tr("UDP 广播服务"), "ok", "Net Server");

    // 初始化定时器 10s 刷新广播
    if (broadcastTimer == nullptr) {
        broadcastTimer = new QTimer(this);
        connect(broadcastTimer, &QTimer::timeout, this, &NetworkWorker::onSendBroadcast);
    }
    onSendBroadcast();
    broadcastTimer->start(10000);

    // 初始化 TCP 监听服务
    if (tcpServer == nullptr) {
        tcpServer = new QTcpServer(this);
        // 监听信号只触发一次
        connect(tcpServer, &QTcpServer::newConnection, this, &NetworkWorker::onClientConnected);
    }
    if (tcpServer->isListening()) {
        tcpServer->close();
    }

    // 在端口 tcpPort: 55520，允许任何局域网 IP 连入
    if (tcpServer->listen(QHostAddress::Any, tcpPort)) {
        emit logMessage(tr("TCP 核心服务"), tr("ok，Tcp 端口: %1").arg(tcpPort), "Net Server");
    } else {
        emit logMessage(tr("TCP 核心服务"), tr("error，端口 %1 可能被占用").arg(tcpPort), "Net Server");
    }
}

// 接收连接
void NetworkWorker::onClientConnected() {
    // Client 连接，则生成专属 Socket
    while (tcpServer->hasPendingConnections()) {
        QTcpSocket *socket = tcpServer->nextPendingConnection();

        clientSockets.append(socket);

        QString ip = socket->peerAddress().toString().remove("::ffff:");
        emit logMessage(tr("新终端连入"), tr("ok，IP 地址：%1").arg(ip), "Net Server");

        // 信号绑定，收到 C 端消息，读取；C 端断开连接，处理断连
        connect(socket, &QTcpSocket::readyRead, this, &NetworkWorker::onRead);
        connect(socket, &QTcpSocket::disconnected, this, &NetworkWorker::onClientDisconnected);
    }
}

void NetworkWorker::onRead() {
    // sender() 获取是谁在说话，也就是触发信号的那个 sender
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return ;

    // 循环读取，防止一次性送来多个包
    while (client->bytesAvailable() > 0) {
        // 1.若固定包头大小都不够，说明数据还没传完，等待
        if (client->bytesAvailable() < sizeof(PACKETHEADER)) {
            return ;
        }

        // 2. 预读包头（仅仅查看）
        PACKETHEADER header;
        client->peek(reinterpret_cast<char*>(&header), sizeof(PACKETHEADER));

        // 3. 魔数校验暗号，若暗号不对，可能是不明攻击
        if (header.magic != MAGICNUM) {
            emit logMessage(tr("未知协议报文"), tr("error，已强制断开连接"), "Net Server");
            client->disconnectFromHost();
            return ;
        }

        // 4. 包头告诉 length 个字节，若当前缓冲区的数据总包长不够（包头+包体），等待
        if (client->bytesAvailable() < (qint64)(sizeof(PACKETHEADER) + header.length)) {
            return ;
        }

        // 5. 可以将所有数据从缓冲区取出
        QByteArray allData = client->read(sizeof(PACKETHEADER) + header.length);
        // 取出真实的业务数据包体
        QByteArray bodyData = allData.mid(sizeof(PACKETHEADER));

        // 根据不同消息类型处理
        if (header.type == MSG_REGISTER_REQ) {
            emit logMessage(tr("[注册请求] 报文"), tr("received，长度：%1").arg(header.length), "Net Server");

            // TODO 交给 DatabaseManager 注册事项
            USERINFO_PACKET *packet = reinterpret_cast<USERINFO_PACKET*>(bodyData.data());

            // 转换为本地业务结构体
            USERINFO info;
            info.username = QString::fromUtf8(packet->username);
            info.pwd = QString::fromUtf8(packet->password);
            info.type = packet->type;

            // 发送给数据库处理
            emit registerRequestToDatabase(client, info);
        } else if (header.type == MSG_LOGIN_REQ) {
            emit logMessage(tr("[登录请求] 报文"), "received", "Net Server");

            // TODO 登录验证事项
        }
    }
}

void NetworkWorker::onClientDisconnected() {
    // C 端断开连接
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    QString ip = client->peerAddress().toString().remove("::ffff:");
    emit logMessage(tr("终端：%1 离线").arg(ip), "ok", "Net Server");

    clientSockets.removeOne(client);// 终端列表删除它
    client->deleteLater();          // 释放内存
}

void NetworkWorker::onReplyRegisterResult(QTcpSocket *client, const STATUS &status) {
    // 检查是否仍在连接
    if (!clientSockets.contains(client)) return;

    // 组装发送给学生的包
    PACKETHEADER header;
    header.magic = MAGICNUM;
    header.length = sizeof(NET_STATUS);
    header.type = MSG_REGISTER_RES;

    // 状态包体
    NET_STATUS reply;
    memset(&reply, 0, sizeof(NET_STATUS));
    reply.code = status.code;
    strncpy_s(reply.msg, status.info.toUtf8().constData(), sizeof(reply.msg) - 1);

    QByteArray buffer;
    buffer.append(reinterpret_cast<const char*>(&header), sizeof(PACKETHEADER));
    buffer.append(reinterpret_cast<const char*>(&reply), sizeof(NET_STATUS));

    qDebug() << "写回学生注册数据";

    client->write(buffer);
    client->disconnectFromHost();
}

void NetworkWorker::onSendBroadcast() {
    SERVERBROADCASTINFO info;
    memset(&info, 0, sizeof(info));

    // 填充魔数暗号
    memset(info.magic, 0, sizeof(info.magic));
    strncpy_s(info.magic, "ROOMATCH", 8);

    // 填充端口
    info.tcpPort = tcpPort;

    // 在线状态
    info.state = BROADCAST_ONLINE;

    // 填充教师名
    memset(info.serverName, 0, sizeof(info.serverName));
    // 将 QString 安全转为 char* 并拷贝
    QByteArray nameBytes = serverName.toUtf8();
    strncpy_s(info.serverName, nameBytes.constData(), sizeof(info.serverName) - 1);

    // 将结构体强转为 char*
    // 随后广播全网 IP(255.255.255.255)，学生端在听端口为 55521
    QByteArray datagram(reinterpret_cast<const char*>(&info), sizeof(info));
    udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, studentListenerPort);
    // udpSocket->writeDatagram(datagram, QHostAddress::LocalHost, studentListenerPort);

    // 测试
    // emit logMessage("UDP 广播心跳包", tr("教师：%1").arg(serverName), "Net Server");
}

void NetworkWorker::onSendLogoutBroadcast() {
    // 停止计时器，不再发送广播
    if (broadcastTimer) {
        broadcastTimer->stop();
    }

    SERVERBROADCASTINFO info;
    memset(&info, 0, sizeof(info));
    strncpy_s(info.magic, "ROOMATCH", 8);
    info.tcpPort = tcpPort;
    QByteArray nameBytes = serverName.toUtf8();
    strncpy_s(info.serverName, nameBytes.constData(), sizeof(info.serverName) - 1);
    info.state = BROADCAST_OFFLINE;

    QByteArray datagram(reinterpret_cast<const char*>(&info), sizeof(info));
    for (int i = 0; i < 3; i++) {
        udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, studentListenerPort);
    }

    for (QTcpSocket *socket : std::as_const(clientSockets)) {
        if (socket && socket->isOpen()) {
            socket->disconnectFromHost();
        }
    }

    if (tcpServer && tcpServer->isListening()) {
        tcpServer->close();
    }

    // 关闭计时器
    if (broadcastTimer && broadcastTimer->isActive()) {
        broadcastTimer->stop();
    }

    emit eventMessage(tr("退出登录"), tr("%1 已登出").arg(serverName));
    // 测试
    emit logMessage("UDP 广播心跳包", tr("端口：%1，教师：%2，已停止").arg(tcpPort).arg(serverName), "Net Server");
}

// 公共接口
void NetworkWorker::setTcpPort(quint16 port) { tcpPort = port; }
void NetworkWorker::setServerName(const QString &name) { serverName = name; }
