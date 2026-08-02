#ifndef DATABASEWORKER_H
#define DATABASEWORKER_H

#include <QObject>
#include <QTcpSocket>
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

    // 注册学生用户
    void onRegisterStudent(QTcpSocket *client, const USERINFO &info);
    // 登录学生用户
    void onLoginStudent(QTcpSocket *client, const USERINFO &info);

    // 保存题目
    void onSaveProblem(const PROBLEMDETAIL &detail);

signals:
    // 所有对数据库核心操作返回的信息
    void logMessage(const QString &event, const QString &statusText, const QString &source);
    // 所有事件返回信息
    void eventMessage(const QString &event, const QString &statusText);

    // 注册用户返回状态
    void registerUserResponse(const STATUS &code);
    // 登录用户返回状态
    void loginUserResponse(const STATUS &code, const QString &username);

    // 学生注册返回信息
    void registerStudentResponse(QTcpSocket *client, const STATUS &code);
    // 学生登录返回信息
    void loginStudentResponse(QTcpSocket *client, const STATUS &code);

    // 题目类型转发
    void problemTagBack(const QVector<QPair<QString, int>> &tags);

    // 保存题目返回状态
    void saveProblemResponse(const STATUS &code);

private:
    int teacherID = 0;
};

#endif // DATABASEWORKER_H
