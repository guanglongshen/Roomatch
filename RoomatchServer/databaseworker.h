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
    // 登录用户函数
    void onLoginUser(const USERINFO &loginInfo);

signals:
    // 所有对数据库核心操作返回的信息
    void logMessage(const QString &event, const QString &statusText, const QString &source);
    // 所有事件返回信息
    void eventMessage(const QString &event, const QString &statusText);

    // 注册用户返回状态
    void registerUserResponse(const STATUS &code);
    // 登录用户返回状态
    void loginUserResponse(const STATUS &code, const QString &username);
};

#endif // DATABASEWORKER_H
