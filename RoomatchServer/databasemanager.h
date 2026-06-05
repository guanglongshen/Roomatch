#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "databaseworker.h"
#include <QObject>
#include <QThread>
#include <tools.h>

class DatabaseManager : public QObject {
    Q_OBJECT
public:
    static DatabaseManager* instance();
    ~DatabaseManager();

    // 启动线程服务
    void startService();

signals:
    // 日志通知
    void logNotify(const QString &event, const QString &statusText, const QString &source);

    // 注册用户请求
    void registerRequest(const USERINFO &registerInfo);
    // 注册用户结果
    void registerResult(const STATUS &status);

private:
    DatabaseManager();
    QThread *managerThread;
    DatabaseWorker *worker;
};

#endif // DATABASEMANAGER_H
