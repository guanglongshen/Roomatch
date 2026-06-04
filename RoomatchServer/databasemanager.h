#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "databaseworker.h"
#include <QObject>
#include <QThread>

class DatabaseManager : public QObject {
    Q_OBJECT
public:
    static DatabaseManager* instance();
    ~DatabaseManager();

    // 启动线程服务
    void startService();

signals:
    void logNotify(const QString &logMsg);

private:
    DatabaseManager();
    QThread *managerThread;
    DatabaseWorker *worker;
};

#endif // DATABASEMANAGER_H
