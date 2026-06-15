#ifndef NETWORKWORKER_H
#define NETWORKWORKER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QHash>
#include <tools.h>

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

    // 外接注册请求转数据库
    void registerRequestToDatabase(QTcpSocket *client, const USERINFO &info);
    // 外接登录请求转数据库
    void loginRequestToDatabase(QTcpSocket *client, const USERINFO &info);

    // 学生的状态改变通知
    void studentStatusChanged(const QString &username, const int &code, const QString &status, const QString &ip);

public slots:
    // 开启网络核心服务
    void onStartNetworkService();

    // Client TCP 连接进来
    void onClientConnected();

    // Client 发送数据
    void onRead();

    // Client 断开连接
    void onClientDisconnected();

    // 注册回执
    void onReplyRegisterResult(QTcpSocket *client, const STATUS &status);
    // 登录回执
    void onReplyLoginResult(QTcpSocket *client, const STATUS &status);

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

    QHash<QString, QTcpSocket*> onlineStudentsMap;  // 快速通过名字找到 Client
};

#endif // NETWORKWORKER_H
