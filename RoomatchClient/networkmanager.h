#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "networkworker.h"
#include "tools.h"
#include <QObject>
#include <QThread>

class NetworkManager : public QObject {
    Q_OBJECT
public:
    static NetworkManager* instance();
    ~NetworkManager();

    // 网络服务开启
    void startService();

signals:
    void teacherDiscoveredUI(const QString &name, const QString &ip, quint16 port);
    void teacherDismissedUI(const QString &name, const QString &ip, quint16 port);

    // 派发给子线程 worker 执行 登录，注册
    void loginRequest(const QString &ip, quint16 port, const USERINFO &info);
    void registerRequest(const QString &ip, quint16 port, const USERINFO &info);

    // 登录注册的信号转发回 UI
    void registerReply(const NET_STATUS &status);
    void loginReply(const NET_STATUS &status);

    // 学生登出信号
    void logoutRequest();
private:
    NetworkManager();
    QThread *managerThread;
    NetworkWorker *worker;
};

#endif // NETWORKMANAGER_H
