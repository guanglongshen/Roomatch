#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "networkworker.h"
#include <QObject>
#include <QThread>

class NetworkManager : public QObject {
    Q_OBJECT
public:
    static NetworkManager* instance();
    ~NetworkManager();

    // 启动网络 Manager 服务，在构造函数结束之后再启动
    void startService();

signals:
    // 转发给 ServerWindow
    void logNotify(const QString &event, const QString &status, const QString &source);

private:
    NetworkManager();
    QThread *netThread = nullptr;
    NetworkWorker *worker = nullptr;
};

#endif // NETWORKMANAGER_H
