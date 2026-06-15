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

    // 公共槽函数
    void requestRegisterStudent(QTcpSocket *client, const USERINFO &info) {
        emit registerStudentRequest(client, info);
    }

signals:
    // 日志通知
    void logNotify(const QString &event, const QString &statusText, const QString &source);
    // 事件通知
    void eventNotify(const QString &event, const QString &statusText);

    // 注册用户请求
    void registerRequest(const USERINFO &registerInfo);
    // 注册用户结果
    void registerResult(const STATUS &status);

    // 登录用户请求
    void loginRequest(const USERINFO &loginInfo);
    // 登录用户结果
    void loginResult(const STATUS &status, const QString &username);

    // 学生注册结果
    void registerStudentResult(QTcpSocket *client, const STATUS &status);
    // 学生注册请求
    void registerStudentRequest(QTcpSocket *client, const USERINFO &info);

    // 学生登录请求
    void loginStudentRequest(QTcpSocket *client, const USERINFO &info);
    // 学生登录结果
    void loginStudentResult(QTcpSocket *client, const STATUS &status);


private:
    DatabaseManager();
    QThread *managerThread;
    DatabaseWorker *worker;
};

#endif // DATABASEMANAGER_H
