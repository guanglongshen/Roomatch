#ifndef NETWORKWORKER_H
#define NETWORKWORKER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>

class NetworkWorker : public QObject {
    Q_OBJECT
public:
    explicit NetworkWorker(QObject *parent = nullptr);
    ~NetworkWorker();

    void setTcpPort(quint16 port);
    void setServerName(const QString &name);

signals:
    // 网络底层动态
    void logMessage(const QString &event, const QString &status, const QString &source);
    // 网络事件动态
    void eventMessage(const QString &event, const QString &status);

public slots:
    // 开启网络核心服务
    void onStartNetworkService();

    // Client TCP 连接进来
    void onClientConnected();

    // Client 发送数据
    void onRead();

    // Client 断开连接
    void onClientDisconnected();

private slots:
    // 定时器：若干秒喊话 发送已登录信息
    void onSendBroadcast();

    // 教师已经登出，发出最后的 UDP 通告
    void onSendLogoutBroadcast();

private:
    QUdpSocket *udpSocket = nullptr;
    QTimer     *broadcastTimer = nullptr;
    quint16     tcpPort = 55520;                    // 默认TCP 监听端口
    QString     serverName = "Default Teacher";     // 默认广播教师名

    QTcpServer *tcpServer = nullptr;
    QList<QTcpSocket *> clientSockets;              // 存放所有 Client
};

#endif // NETWORKWORKER_H
