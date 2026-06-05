#ifndef DATABASEWORKER_H
#define DATABASEWORKER_H

#include <QObject>
#include <tools.h>

class DatabaseWorker : public QObject {
    Q_OBJECT
public:
    explicit DatabaseWorker(QObject *parent = nullptr);

    // 初始化数据库
    void onInitializeDatabase();

    // 注册用户函数
    void onRegisterUser(const USERINFO &registerInfo);
signals:
    // 所有对数据库核心操作返回的信息
    void logMessage(const QString &msg);

    // 注册用户返回状态
    void registerUserResponse(const STATUS &code);
};

#endif // DATABASEWORKER_H
