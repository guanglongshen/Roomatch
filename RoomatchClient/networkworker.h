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

    // 登录请求
    void onSendLoginRequest(const QString &ip, quint16 port, const USERINFO &info);
    void onSendRegisterRequest(const QString &ip, quint16 port, const USERINFO &info);

private slots:
    void onReadPendingDatagrams();  // UDP广播监听
    void onTcpReadyRead();          // TCP回复监听

signals:
    // 发现教师
    void teacherDiscovered(const QString &name, const QString &ip, quint16 port);
    // 教师离线
    void teacherDismissed(const QString &name, const QString &ip, quint16 port);

    // 登录结果，注册结果
    void loginResponse(const NET_STATUS &status);
    void registerResponse(const NET_STATUS &status);

private:
    QUdpSocket *udpListener = nullptr;
    QTcpSocket *tcpClient = nullptr;
    quint16 teacherPort = 55520;
};

#endif // NETWORKWORKER_H
