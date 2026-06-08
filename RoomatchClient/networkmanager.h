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
    void loginResultUI(const STATUS &status);
    void registerResultUI(const STATUS &status);

    // 派发给子线程 worker 执行
    void loginRequest(const QString &ip, quint16 port, const USERINFO &info);
    void registerRequest(const QString &ip, quint16 port, const USERINFO &info);

private:
    NetworkManager();
    QThread *managerThread;
    NetworkWorker *worker;
};

#endif // NETWORKMANAGER_H
