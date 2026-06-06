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

    // 测试
    emit logMessage(tr("UDP 广播服务"), "ok", "Net Server");

    // 初始化 TCP 监听服务
    tcpServer = new QTcpServer(this);

    // 在端口 tcpPort: 55520，允许任何局域网 IP 连入
    if (tcpServer->listen(QHostAddress::Any, tcpPort)) {
        connect(tcpServer, &QTcpServer::newConnection, this, &NetworkWorker::onClientConnected);
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

void NetworkWorker::onSendBroadcast() {
    SERVERBROADCASTINFO info;

    // 填充魔数暗号
    memset(info.magic, 0, sizeof(info.magic));
    strncpy_s(info.magic, "ROOMATCH", 8);

    // 填充端口
    info.tcpPort = tcpPort;

    // 填充教师名
    memset(info.serverName, 0, sizeof(info.serverName));
    // 将 QString 安全转为 char* 并拷贝
    QByteArray nameBytes = serverName.toUtf8();
    strncpy_s(info.serverName, nameBytes, sizeof(info.serverName) - 1);

    // 将结构体强转为 char*
    // 随后广播全网 IP(255.255.255.255)，学生端在听端口为 55521
    QByteArray datagram(reinterpret_cast<const char*>(&info), sizeof(info));
    udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, 55521);

    // 测试
    emit logMessage("UDP 广播心跳包", tr("端口：%1，教师：%2").arg(tcpPort).arg(serverName), "Net Server");
}

// 公共接口
void NetworkWorker::setTcpPort(quint16 port) { tcpPort = port; }
void NetworkWorker::setServerName(const QString &name) { serverName = name; }
