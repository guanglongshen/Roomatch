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

    void setTeacherName(const QString &name);

    // 启动网络 Manager 服务，在构造函数结束之后再启动
    void startService();

    // 关闭网络 Manager 服务
    void stopService();

    // 将worker 暴露于公共
    NetworkWorker* getWorker() const { return worker; }

signals:
    // 转发给 ServerWindow
    void logNotify(const QString &event, const QString &status, const QString &source);
    // 转发给 ServerWindow 网络事件
    void eventNotify(const QString &event, const QString &status);

private:
    NetworkManager();
    QThread *netThread = nullptr;
    NetworkWorker *worker = nullptr;
};

#endif // NETWORKMANAGER_H
