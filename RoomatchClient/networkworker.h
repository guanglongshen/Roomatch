#ifndef NETWORKWORKER_H
#define NETWORKWORKER_H

#include "tools.h"
#include <QObject>
#include <QUdpSocket>
#include <QTcpSocket>
#include <protocols.h>

class NetworkWorker : public QObject {
    Q_OBJECT
public:
    explicit NetworkWorker(QObject *parent = nullptr);

public slots:
    // 初始化套接字
    void onInitializeNetwork();

    // 登录请求 注册请求
    void onSendLoginRequest(const QString &ip, quint16 port, const USERINFO &info);
    void onSendRegisterRequest(const QString &ip, quint16 port, const USERINFO &info);

    // 登出通讯，因为已经连接上了，不需要 ip prot 去连接了
    void onSendLogoutRequest();

private slots:
    void onReadPendingDatagrams();  // UDP广播监听
    void onTcpReadyRead();          // TCP回复监听

    // 心跳包
    void onSendHeartBeat();

signals:
    // 发现教师
    void teacherDiscovered(const QString &name, const QString &ip, quint16 port);
    // 教师离线
    void teacherDismissed(const QString &name, const QString &ip, quint16 port);

    // 登录结果，注册结果
    void loginResponse(const NET_STATUS &status);
    void registerResponse(const NET_STATUS &status);

    // 强制登出信号
    void forceLogoutResponse();

private:
    QUdpSocket *udpListener = nullptr;
    QTcpSocket *tcpClient = nullptr;
    quint16 teacherPort = 55520;

    QTimer *heartbeatTimer = nullptr;   // 心跳包计时器
};

#endif // NETWORKWORKER_H
