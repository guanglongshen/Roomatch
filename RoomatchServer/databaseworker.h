#ifndef DATABASEWORKER_H
#define DATABASEWORKER_H

#include <QObject>

class DatabaseWorker : public QObject {
    Q_OBJECT
public:
    explicit DatabaseWorker(QObject *parent = nullptr);

    void onInitializeDatabase();
signals:
    void logMessage(const QString &msg);           // 所有操作返回的信息
};

#endif // DATABASEWORKER_H
